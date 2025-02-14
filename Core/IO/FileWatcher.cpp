#include "Predefined.h"

#include "File.h"
#include "FileSystem.h"
#include "FileWatcher.h"
#include "Log.h"

#ifdef WIN32
#include <windows.h>
#elif __linux__
#include <sys/inotify.h>
extern "C"
{
	// Need read/close for inotify
#include "unistd.h"
}
#elif defined(__APPLE__) && !defined(IOS)
extern "C"
{
#include "../../IO/include/MacFileWatcher.h"
}
#endif


namespace Sapphire
{

#ifndef __APPLE__
	static const unsigned BUFFERSIZE = 4096;
#endif

	FileWatcher::FileWatcher(Context* context) :
		Object(context),
		fileSystem_(GetSubsystem<FileSystem>()),
		delay_(1.0f),
		watchSubDirs_(false)
	{
#if defined(SAPPHIRE_FILEWATCHER)
#if defined(__linux__)
		watchHandle_ = inotify_init();
#elif defined(__APPLE__) && !defined(IOS)
		supported_ = IsFileWatcherSupported();
#endif
#endif
	}

	FileWatcher::~FileWatcher()
	{
		StopWatching();
#if defined(SAPPHIRE_FILEWATCHER)
#if defined(__linux__)
		close(watchHandle_);
#endif
#endif
	}

	bool FileWatcher::StartWatching(const String& pathName, bool watchSubDirs)
	{
		if (!fileSystem_)
		{
			SAPPHIRE_LOGERROR("No FileSystem, can not start watching");
			return false;
		}

		// 先停止之前的观察
		StopWatching();

#if defined(SAPPHIRE_FILEWATCHER) && defined(SAPPHIRE_THREADING)
#if defined(WIN32)
		String nativePath = GetNativePath(RemoveTrailingSlash(pathName));

		//调windows 本地函数
		//获取目录句柄
		dirHandle_ = (void*)CreateFileW(
			WString(nativePath).CString(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
			0,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			0);

		if (dirHandle_ != INVALID_HANDLE_VALUE)
		{
			path_ = AddTrailingSlash(pathName);
			watchSubDirs_ = watchSubDirs;
			Run();

			SAPPHIRE_LOGDEBUG("Started watching path " + pathName);
			return true;
		}
		else
		{
			//目录不存在
			SAPPHIRE_LOGERROR("Failed to start watching path " + pathName);
			return false;
		}
#elif defined(__linux__)
		int flags = IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO;
		int handle = inotify_add_watch(watchHandle_, pathName.CString(), (uint32_t)flags);

		if (handle < 0)
		{
			SAPPHIRE_LOGERROR("Failed to start watching path " + pathName);
			return false;
		}
		else
		{
			// Store the root path here when reconstructed with inotify later
			dirHandle_[handle] = "";
			path_ = AddTrailingSlash(pathName);
			watchSubDirs_ = watchSubDirs;

			if (watchSubDirs_)
			{
				Vector<String> subDirs;
				fileSystem_->ScanDir(subDirs, pathName, "*", SCAN_DIRS, true);

				for (unsigned i = 0; i < subDirs.Size(); ++i)
				{
					String subDirFullPath = AddTrailingSlash(path_ + subDirs[i]);

					// Don't watch ./ or ../ sub-directories
					if (!subDirFullPath.EndsWith("./"))
					{
						handle = inotify_add_watch(watchHandle_, subDirFullPath.CString(), (uint32_t)flags);
						if (handle < 0)
							SAPPHIRE_LOGERROR("Failed to start watching subdirectory path " + subDirFullPath);
						else
						{
							// Store sub-directory to reconstruct later from inotify
							dirHandle_[handle] = AddTrailingSlash(subDirs[i]);
						}
					}
				}
			}
			Run();

			SAPPHIRE_LOGDEBUG("Started watching path " + pathName);
			return true;
		}
#elif defined(__APPLE__) && !defined(IOS)
		if (!supported_)
		{
			SAPPHIRE_LOGERROR("Individual file watching not supported by this OS version, can not start watching path " + pathName);
			return false;
		}

		watcher_ = CreateFileWatcher(pathName.CString(), watchSubDirs);
		if (watcher_)
		{
			path_ = AddTrailingSlash(pathName);
			watchSubDirs_ = watchSubDirs;
			Run();

			SAPPHIRE_LOGDEBUG("Started watching path " + pathName);
			return true;
		}
		else
		{
			SAPPHIRE_LOGERROR("Failed to start watching path " + pathName);
			return false;
		}
#else
		SAPPHIRE_LOGERROR("FileWatcher not implemented, can not start watching path " + pathName);
		return false;
#endif
#else
		SAPPHIRE_LOGDEBUG("FileWatcher feature not enabled");
		return false;
#endif
	}

	void FileWatcher::StopWatching()
	{
		if (handle_)
		{
			shouldRun_ = false;

			// Create and delete a dummy file to make sure the watcher loop terminates
			String dummyFileName = path_ + "dummy.tmp";
			File file(context_, dummyFileName, FILE_WRITE);
			file.Close();
			if (fileSystem_)
				fileSystem_->Delete(dummyFileName);

			Stop();

#if defined(WIN32)
			CloseHandle((HANDLE)dirHandle_);
#elif defined(__linux__)
			for (HashMap<int, String>::Iterator i = dirHandle_.Begin(); i != dirHandle_.End(); ++i)
				inotify_rm_watch(watchHandle_, i->first_);
			dirHandle_.Clear();
#elif defined(__APPLE__) && !defined(IOS)
			CloseFileWatcher(watcher_);
#endif

			SAPPHIRE_LOGDEBUG("Stopped watching path " + path_);
			path_.Clear();
		}
	}

	void FileWatcher::SetDelay(float interval)
	{
		delay_ = Max(interval, 0.0f);
	}

	void FileWatcher::ThreadFunction()
	{
#if defined(SAPPHIRE_FILEWATCHER)
#if defined(WIN32)
		unsigned char buffer[BUFFERSIZE];
		DWORD bytesFilled = 0;

		while (shouldRun_)
		{
			//读取目录的改变信息
			if (ReadDirectoryChangesW((HANDLE)dirHandle_,
				buffer,
				BUFFERSIZE,
				watchSubDirs_,
				FILE_NOTIFY_CHANGE_FILE_NAME |
				FILE_NOTIFY_CHANGE_LAST_WRITE,
				&bytesFilled,
				0,
				0))
			{
				unsigned offset = 0;

				while (offset < bytesFilled)
				{
					FILE_NOTIFY_INFORMATION* record = (FILE_NOTIFY_INFORMATION*)&buffer[offset];

					if (record->Action == FILE_ACTION_MODIFIED || record->Action == FILE_ACTION_RENAMED_NEW_NAME)
					{
						String fileName;
						const wchar_t* src = record->FileName;
						const wchar_t* end = src + record->FileNameLength / 2;
						while (src < end)
							fileName.AppendUTF8(String::DecodeUTF16(src));

						fileName = GetInternalPath(fileName);
						AddChange(fileName);
					}

					if (!record->NextEntryOffset)
						break;
					else
						offset += record->NextEntryOffset;
				}
			}
		}
#elif defined(__linux__)
		unsigned char buffer[BUFFERSIZE];

		while (shouldRun_)
		{
			int i = 0;
			int length = (int)read(watchHandle_, buffer, sizeof(buffer));

			if (length < 0)
				return;

			while (i < length)
			{
				inotify_event* event = (inotify_event*)&buffer[i];

				if (event->len > 0)
				{
					if (event->mask & IN_MODIFY || event->mask & IN_MOVE)
					{
						String fileName;
						fileName = dirHandle_[event->wd] + event->name;
						AddChange(fileName);
					}
				}

				i += sizeof(inotify_event) + event->len;
			}
		}
#elif defined(__APPLE__) && !defined(IOS)
		while (shouldRun_)
		{
			Time::Sleep(100);

			String changes = ReadFileWatcher(watcher_);
			if (!changes.Empty())
			{
				Vector<String> fileNames = changes.Split(1);
				for (unsigned i = 0; i < fileNames.Size(); ++i)
					AddChange(fileNames[i]);
			}
		}
#endif
#endif
	}

	void FileWatcher::AddChange(const String& fileName)
	{
		MutexLock lock(changesMutex_);

		// 重载相关文件名的计时器。 
		changes_[fileName].Reset();
	}

	bool FileWatcher::GetNextChange(String& dest)
	{
		MutexLock lock(changesMutex_);

		unsigned delayMsec = (unsigned)(delay_ * 1000.0f);

		if (changes_.Empty())
			return false;
		else
		{
			for (HashMap<String, Timer>::Iterator i = changes_.Begin(); i != changes_.End(); ++i)
			{
				if (i->second_.GetMSec(false) >= delayMsec)
				{
					dest = i->first_;
					changes_.Erase(i);
					return true;
				}
			}

			return false;
		}
	}

}