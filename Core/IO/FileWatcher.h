#pragma once

#include "List.h"
#include "../Mutex.h"
#include "../Object.h"
#include "../Thread.h"
#include "../Timer.h"


namespace Sapphire
{

	class FileSystem;

	// �۲�һ��Ŀ¼��������Ŀ¼���ļ��Ƿ��޸�
	class SAPPHIRE_API FileWatcher : public Object, public Thread
	{
		SAPPHIRE_OBJECT(FileWatcher, Object);

	public:
		 
		FileWatcher(Context* context);
		 
		virtual ~FileWatcher();

		//  Ŀ¼�۲�ѭ��
		virtual void ThreadFunction();

		// ��ʼ�۲�һ��Ŀ¼�� ����ɹ�����true
		bool StartWatching(const String& pathName, bool watchSubDirs);
		/// ����Ŀ¼�۲�
		void StopWatching();
		//�������ļ��ı�֪ͨǰ���ӳ�ʱ�䡣����Ա��⵱һ���ļ���Ȼ���ڱ�������У�Ĭ��1��
		void SetDelay(float interval);
		//  ���һ���ļ����ı�����
		void AddChange(const String& fileName);
		// ����һ���ļ��ĸı䣨���δ�ҵ�������false)
		bool GetNextChange(String& dest);

		/// ���ر��۲��Ŀ¼
		const String& GetPath() const { return path_; }

		/// �����ļ��ı���ӳ�֪ͨ�ĺ�����
		float GetDelay() const { return delay_; }

	private:
		/// �ļ�ϵͳ
		SharedPtr<FileSystem> fileSystem_;
		/// �۲�·��
		String path_;
		/// �ȴ��ı䣬�����ǵ�timer��ʱ������б��з��غ��Ƴ�
		HashMap<String, Timer> changes_;
		/// �������ı�Ļ�����
		Mutex changesMutex_;
		/// �ı��֪ͨ���ӳ�����
		float delay_;
		/// �Ƿ�۲���Ŀ¼
		bool watchSubDirs_;

#ifdef WIN32

		/// �۲��Ŀ¼���
		void* dirHandle_;

#elif __linux__

		/// Ŀ¼����Ŀ¼�ľ�� 
		HashMap<int, String> dirHandle_;
		/// linux��Ҫ�ľ��
		int watchHandle_;

#elif defined(__APPLE__) && !defined(IOS)

		/// �Ƿ���������OS�����ļ��۲��־
		bool supported_;
		///ָ��MacFileWatcher�ڲ������ָ��
		void* watcher_;

#endif
	};
}