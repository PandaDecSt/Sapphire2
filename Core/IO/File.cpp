#include "Predefined.h"
#include "../Core/profiler.h"
#include "../Core/Context.h"
#include "../Core/CoreEvents.h"
#include "File.h"
#include "FileSystem.h"
#include "Log.h"
#include "MemoryBuffer.h"
#include "PackageFile.h"


#include <cstdio>
#include "LZ4/lz4.h"

#include "DebugNew.h"


namespace Sapphire
{

#ifdef WIN32
	static const wchar_t* openMode[] =
	{
		L"rb",
		L"wb",
		L"r+b",
		L"w+b"
	};
#else
	static const char* openMode[] =
	{
		"rb",
		"wb",
		"r+b",
		"w+b"
	};
#endif

#ifdef ANDROID
	const char* APK = "/apk/";
	static const unsigned READ_BUFFER_SIZE = 32768;
#endif
	static const unsigned SKIP_BUFFER_SIZE = 1024;

	File::File(Context* context) :
		Object(context),
		mode_(FILE_READ),
		handle_(0),
#ifdef ANDROID
		assetHandle_(0),
#endif
		readBufferOffset_(0),
		readBufferSize_(0),
		offset_(0),
		checksum_(0),
		compressed_(false),
		readSyncNeeded_(false),
		writeSyncNeeded_(false)
	{
	}

	File::File(Context* context, const String& fileName, FileMode mode) :
		Object(context),
		mode_(FILE_READ),
		handle_(0),
#ifdef ANDROID
		assetHandle_(0),
#endif
		readBufferOffset_(0),
		readBufferSize_(0),
		offset_(0),
		checksum_(0),
		compressed_(false),
		readSyncNeeded_(false),
		writeSyncNeeded_(false)
	{
		Open(fileName, mode);
	}

	File::File(Context* context, PackageFile* package, const String& fileName) :
		Object(context),
		mode_(FILE_READ),
		handle_(0),
#ifdef ANDROID
		assetHandle_(0),
#endif
		readBufferOffset_(0),
		readBufferSize_(0),
		offset_(0),
		checksum_(0),
		compressed_(false),
		readSyncNeeded_(false),
		writeSyncNeeded_(false)
	{
		Open(package, fileName);
	}

	File::~File()
	{
		Close();
	}

	bool File::Open(const String& fileName, FileMode mode)
	{
		Close();

		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		if (fileSystem && !fileSystem->CheckAccess(GetPath(fileName)))
		{
			SAPPHIRE_LOGERRORF("Access denied to %s", fileName.CString());
			return false;
		}

#ifdef ANDROID
		if (SAPPHIRE_IS_ASSET(fileName))
		{
			if (mode != FILE_READ)
			{
				SAPPHIRE_LOGERROR("Only read mode is supported for asset files");
				return false;
			}

			assetHandle_ = SDL_RWFromFile(SAPPHIRE_ASSET(fileName), "rb");
			if (!assetHandle_)
			{
				SAPPHIRE_LOGERRORF("Could not open asset file %s", fileName.CString());
				return false;
			}
			else
			{
				fileName_ = fileName;
				mode_ = mode;
				position_ = 0;
				offset_ = 0;
				checksum_ = 0;
				size_ = assetHandle_->hidden.androidio.size;
				readBuffer_ = new unsigned char[READ_BUFFER_SIZE];
				readBufferOffset_ = 0;
				readBufferSize_ = 0;
				return true;
			}
		}
#endif

		if (fileName.Empty())
		{
			SAPPHIRE_LOGERROR("Could not open file with empty name");
			return false;
		}

#ifdef WIN32
		handle_ = _wfopen(GetWideNativePath(fileName).CString(), openMode[mode]);
#else
		handle_ = fopen(GetNativePath(fileName).CString(), openMode[mode]);
#endif

		// If file did not exist in readwrite mode, retry with write-update mode
		if (mode == FILE_READWRITE && !handle_)
		{
#ifdef WIN32
			handle_ = _wfopen(GetWideNativePath(fileName).CString(), openMode[mode + 1]);
#else
			handle_ = fopen(GetNativePath(fileName).CString(), openMode[mode + 1]);
#endif
		}

		if (!handle_)
		{
			SAPPHIRE_LOGERRORF("Could not open file %s", fileName.CString());
			return false;
		}

		fileName_ = fileName;
		mode_ = mode;
		position_ = 0;
		offset_ = 0;
		checksum_ = 0;
		compressed_ = false;
		readSyncNeeded_ = false;
		writeSyncNeeded_ = false;

		fseek((FILE*)handle_, 0, SEEK_END);
		long size = ftell((FILE*)handle_);
		fseek((FILE*)handle_, 0, SEEK_SET);
		if (size > M_MAX_UNSIGNED)
		{
			SAPPHIRE_LOGERRORF("Could not open file %s which is larger than 4GB", fileName.CString());
			Close();
			size_ = 0;
			return false;
		}
		size_ = (unsigned)size;
		return true;
	}

	bool File::Open(PackageFile* package, const String& fileName)
	{
		Close();

		if (!package)
			return false;
		//包入口
		const PackageEntry* entry = package->GetEntry(fileName);
		if (!entry)
			return false;
		//打开package文件
#ifdef WIN32
		handle_ = _wfopen(GetWideNativePath(package->GetName()).CString(), L"rb");
#else
		handle_ = fopen(GetNativePath(package->GetName()).CString(), "rb");
#endif
		if (!handle_)
		{
			SAPPHIRE_LOGERROR("Could not open package file " + fileName);
			return false;
		}

		fileName_ = fileName;
		mode_ = FILE_READ;
		//设置大入口所指定的偏移地址
		offset_ = entry->offset_;
		checksum_ = entry->checksum_;
		position_ = 0;
		//package的大小
		size_ = entry->size_;
		compressed_ = package->IsCompressed();
		readSyncNeeded_ = false;
		writeSyncNeeded_ = false;

		fseek((FILE*)handle_, offset_, SEEK_SET);
		return true;
	}

	unsigned File::Read(void* dest, unsigned size)
	{
#ifdef ANDROID
		if (!handle_ && !assetHandle_)
#else
		if (!handle_)
#endif
		{
			// Do not log the error further here to prevent spamming the stderr stream
			return 0;
		}

		if (mode_ == FILE_WRITE)
		{
			SAPPHIRE_LOGERROR("File not opened for reading");
			return 0;
		}

		if (size + position_ > size_)
			size = size_ - position_;
		if (!size)
			return 0;

#ifdef ANDROID
		if (assetHandle_)
		{
			unsigned sizeLeft = size;
			unsigned char* destPtr = (unsigned char*)dest;

			while (sizeLeft)
			{
				if (readBufferOffset_ >= readBufferSize_)
				{
					readBufferSize_ = Min((int)size_ - position_, (int)READ_BUFFER_SIZE);
					readBufferOffset_ = 0;
					SDL_RWread(assetHandle_, readBuffer_.Get(), readBufferSize_, 1);
				}

				unsigned copySize = Min((int)(readBufferSize_ - readBufferOffset_), (int)sizeLeft);
				memcpy(destPtr, readBuffer_.Get() + readBufferOffset_, copySize);
				destPtr += copySize;
				sizeLeft -= copySize;
				readBufferOffset_ += copySize;
				position_ += copySize;
			}

			return size;
		}
#endif
		if (compressed_)
		{
			//压缩标志
			unsigned sizeLeft = size;   //剩余字节
			unsigned char* destPtr = (unsigned char*)dest;     //目标地址指针

			while (sizeLeft)
			{
				if (!readBuffer_ || readBufferOffset_ >= readBufferSize_)
				{
					//读取文件头块
					unsigned char blockHeaderBytes[4];
					fread(blockHeaderBytes, sizeof blockHeaderBytes, 1, (FILE*)handle_);
					//构造一个内存流
					MemoryBuffer blockHeader(&blockHeaderBytes[0], sizeof blockHeaderBytes);
					unsigned unpackedSize = blockHeader.ReadUShort();   //读取未压缩的大小
					unsigned packedSize = blockHeader.ReadUShort();     //读取压缩的大小

					if (!readBuffer_)
					{
						readBuffer_ = new unsigned char[unpackedSize];     //未压缩的缓冲区
						inputBuffer_ = new unsigned char[LZ4_compressBound(unpackedSize)];    //用于解压缩的缓冲区
					}
					//读取要解压缩的数据
					fread(inputBuffer_.Get(), packedSize, 1, (FILE*)handle_);
					//将压缩缓冲区内容解压到
					LZ4_decompress_fast((const char*)inputBuffer_.Get(), (char*)readBuffer_.Get(), unpackedSize);

					readBufferSize_ = unpackedSize;
					readBufferOffset_ = 0;
				}
				//计算复制的大小
				unsigned copySize = (unsigned)Min((int)(readBufferSize_ - readBufferOffset_), (int)sizeLeft);
				memcpy(destPtr, readBuffer_.Get() + readBufferOffset_, copySize);
				destPtr += copySize;
				//计算剩余大小
				sizeLeft -= copySize;
				//新的缓冲区偏移地址
				readBufferOffset_ += copySize;
				position_ += copySize;
			}

			return size;
		}

		// 同步读标志，需要重新关联内部缓冲区位置
		if (readSyncNeeded_)
		{
			fseek((FILE*)handle_, position_ + offset_, SEEK_SET);
			readSyncNeeded_ = false;
		}

		size_t ret = fread(dest, size, 1, (FILE*)handle_);
		if (ret != 1)
		{
			fseek((FILE*)handle_, position_ + offset_, SEEK_SET);
			SAPPHIRE_LOGERROR("Error while reading from file " + GetName());
			return 0;
		}

		writeSyncNeeded_ = true;
		position_ += size;
		return size;
	}

	unsigned File::Seek(unsigned position)
	{
#ifdef ANDROID
		if (!handle_ && !assetHandle_)
#else
		if (!handle_)
#endif
		{
			// Do not log the error further here to prevent spamming the stderr stream
			return 0;
		}

		// Allow sparse seeks if writing
		if (mode_ == FILE_READ && position > size_)
			position = size_;

#ifdef ANDROID
		if (assetHandle_)
		{
			SDL_RWseek(assetHandle_, position, SEEK_SET);
			position_ = position;
			readBufferOffset_ = 0;
			readBufferSize_ = 0;
			return position_;
		}
#endif
		if (compressed_)
		{
			// Start over from the beginning
			if (position == 0)
			{
				position_ = 0;
				readBufferOffset_ = 0;
				readBufferSize_ = 0;
				fseek((FILE*)handle_, offset_, SEEK_SET);
			}
			// Skip bytes
			else if (position >= position_)
			{
				unsigned char skipBuffer[SKIP_BUFFER_SIZE];
				while (position > position_)
					Read(skipBuffer, (unsigned)Min((int)position - position_, (int)SKIP_BUFFER_SIZE));
			}
			else
				SAPPHIRE_LOGERROR("Seeking backward in a compressed file is not supported");

			return position_;
		}

		fseek((FILE*)handle_, position + offset_, SEEK_SET);
		position_ = position;
		readSyncNeeded_ = false;
		writeSyncNeeded_ = false;
		return position_;
	}

	unsigned File::Write(const void* data, unsigned size)
	{
		if (!handle_)
		{
			// Do not log the error further here to prevent spamming the stderr stream
			return 0;
		}

		if (mode_ == FILE_READ)
		{
			SAPPHIRE_LOGERROR("File not opened for writing");
			return 0;
		}

		if (!size)
			return 0;

		// Need to reassign the position due to internal buffering when transitioning from reading to writing
		if (writeSyncNeeded_)
		{
			fseek((FILE*)handle_, position_ + offset_, SEEK_SET);
			writeSyncNeeded_ = false;
		}

		if (fwrite(data, size, 1, (FILE*)handle_) != 1)
		{
			// Return to the position where the write began
			fseek((FILE*)handle_, position_ + offset_, SEEK_SET);
			SAPPHIRE_LOGERROR("Error while writing to file " + GetName());
			return 0;
		}

		readSyncNeeded_ = true;
		position_ += size;
		if (position_ > size_)
			size_ = position_;

		return size;
	}

	unsigned File::GetChecksum()
	{
		if (offset_ || checksum_)
			return checksum_;
#ifdef ANDROID
		if ((!handle_ && !assetHandle_) || mode_ == FILE_WRITE)
#else
		if (!handle_ || mode_ == FILE_WRITE)
#endif
			return 0;

		SAPPHIRE_PROFILE(CalculateFileChecksum);

		unsigned oldPos = position_;
		checksum_ = 0;

		Seek(0);
		while (!IsEof())
		{
			unsigned char block[1024];
			unsigned readBytes = Read(block, 1024);
			for (unsigned i = 0; i < readBytes; ++i)
				checksum_ = SDBMHash(checksum_, block[i]);
		}

		Seek(oldPos);
		return checksum_;
	}

	void File::Close()
	{
#ifdef ANDROID
		if (assetHandle_)
		{
			SDL_RWclose(assetHandle_);
			assetHandle_ = 0;
		}
#endif

		readBuffer_.Reset();
		inputBuffer_.Reset();

		if (handle_)
		{
			fclose((FILE*)handle_);
			handle_ = 0;
			position_ = 0;
			size_ = 0;
			offset_ = 0;
			checksum_ = 0;
		}
	}

	void File::Flush()
	{
		if (handle_)
			fflush((FILE*)handle_);
	}

	void File::SetName(const String& name)
	{
		fileName_ = name;
	}

	bool File::IsOpen() const
	{
#ifdef ANDROID
		return handle_ != 0 || assetHandle_ != 0;
#else
		return handle_ != 0;
#endif
	}
}