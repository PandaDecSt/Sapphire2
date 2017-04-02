#pragma once

#include "ArrayPtr.h"
#include "Serializer.h"
#include "Deserializer.h"
#include "../Object.h"


#ifdef ANDROID
#include <SDL/SDL_rwops.h>
#endif

namespace Sapphire
{

#ifdef ANDROID
	extern const char* APK;

	//�����ĵ�·�����Ƿ���APK����ԴĿ¼��
#define SAPPHIRE_IS_ASSET(p) p.StartsWith(APK)
	// ��ȡAPKǰ׺�ĺ�
#ifdef ASSET_DIR_INDICATOR
#define SAPPHIRE_ASSET(p) p.Substring(5).Replaced("/", ASSET_DIR_INDICATOR "/").CString()
#else
#define SAPPHIRE_ASSET(p) p.Substring(5).CString()
#endif
#endif

	//�ļ��Ĵ�ģʽ
	enum FileMode
	{
		FILE_READ = 0,
		FILE_WRITE,
		FILE_READWRITE
	};

	class PackageFile;

	//�ļ�ͨ���ļ�ϵͳ����package�ļ���
	class SAPPHIRE_API File : public Object, public Deserializer, public Serializer
	{
		SAPPHIRE_OBJECT(File, Object);

	public:
		 
		File(Context* context);
		/// ��һ���ļ�ϵͳ�ļ�
		File(Context* context, const String& fileName, FileMode mode = FILE_READ);
		/// �Ӱ��ļ��� 
		File(Context* context, PackageFile* package, const String& fileName);
		/// ����
		virtual ~File();

		//���ļ���ȡ�Լ�������ʵ�ʶ�ȡ���ֽ���
		virtual unsigned Read(void* dest, unsigned size);
		//  �����ļ���ʼ��ƫ��λ��
		virtual unsigned Seek(unsigned position);
		// д�ֽڵ��ļ�������ʵ��д����ֽ���
		virtual unsigned Write(const void* data, unsigned size);

		// �����ļ���
		virtual const String& GetName() const { return fileName_; }

		//����ļ�У���
		virtual unsigned GetChecksum();

	   //��һ���ļ�ϵͳ�ļ�������ɹ�����true
		bool Open(const String& fileName, FileMode mode = FILE_READ);
		//��һ��Package������ɹ�����true
		bool Open(PackageFile* package, const String& fileName);
		 
		void Close();
		// ˢ�������ѻ��������������ļ�
		void Flush();
	 
		//�ı��ļ�����������Դϵͳ
		void SetName(const String& name);

		/// ���ش�ģʽ
		FileMode GetMode() const { return mode_; }

		/// �����Ƿ��
		bool IsOpen() const;

		/// �����ļ����
		void* GetHandle() const { return handle_; }

		/// ��������ļ��Ƿ��һ������
		bool IsPackaged() const { return offset_ != 0; }

	private:
		/// �ļ���
		String fileName_;
		/// ��ģʽ.
		FileMode mode_;
		/// �ļ����
		void* handle_;
#ifdef ANDROID
		// ��׿��Դ���ص�SDL RWops ����
		SDL_RWops* assetHandle_;
#endif
		 
		//��ȡ��׿��Դ�����ѹ���ļ��Ļ�����
		SharedArrayPtr<unsigned char> readBuffer_;
		///����ѹ���ļ��Ľ�ѹ�����뻺����
		SharedArrayPtr<unsigned char> inputBuffer_;
		// ��ȡ��������ƫ��λ��
		unsigned readBufferOffset_;
		/// ��ǰ��ȡ�������Ĵ�С
		unsigned readBufferSize_;
		/// һ��package�ļ�����ʼλ�ã�0��ʾ�����ļ�
		unsigned offset_;
		/// У�������
		unsigned checksum_;
		/// ѹ����־
		bool compressed_;
		///  �Ƿ�ͬ����ȡ
		bool readSyncNeeded_;
		///  �Ƿ�ͬ��д��
		bool writeSyncNeeded_;
	};

}