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

		/// Return the path being watched, or empty if not watching.
		const String& GetPath() const { return path_; }

		/// Return the delay in seconds for notifying file changes.
		float GetDelay() const { return delay_; }

	private:
		/// Filesystem.
		SharedPtr<FileSystem> fileSystem_;
		/// The path being watched.
		String path_;
		/// Pending changes. These will be returned and removed from the list when their timer has exceeded the delay.
		HashMap<String, Timer> changes_;
		/// Mutex for the change buffer.
		Mutex changesMutex_;
		/// Delay in seconds for notifying changes.
		float delay_;
		/// Watch subdirectories flag.
		bool watchSubDirs_;

#ifdef WIN32

		/// Directory handle for the path being watched.
		void* dirHandle_;

#elif __linux__

		/// HashMap for the directory and sub-directories (needed for inotify's int handles).
		HashMap<int, String> dirHandle_;
		/// Linux inotify needs a handle.
		int watchHandle_;

#elif defined(__APPLE__) && !defined(IOS)

		/// Flag indicating whether the running OS supports individual file watching.
		bool supported_;
		/// Pointer to internal MacFileWatcher delegate.
		void* watcher_;

#endif
	};
}