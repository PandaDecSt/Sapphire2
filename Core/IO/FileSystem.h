#pragma once

#include "HashSet.h"
#include "List.h"
#include "../Object.h"


namespace Sapphire
{

	class AsyncExecRequest;

	/// �����ļ�
	static const unsigned SCAN_FILES = 0x1;
	/// ����Ŀ¼
	static const unsigned SCAN_DIRS = 0x2;
	/// ���������ļ�
	static const unsigned SCAN_HIDDEN = 0x4;

	// �ļ���Ŀ¼�����ͷ��ʿ��Ƶ���ϵͳ
	class SAPPHIRE_API FileSystem : public Object
	{
		SAPPHIRE_OBJECT(FileSystem, Object);

	public:
		 
		FileSystem(Context* context);

		~FileSystem();

		/// ���õ�ǰ����Ŀ¼
		bool SetCurrentDir(const String& pathName);
		/// ����һ��Ŀ¼
		bool CreateDir(const String& pathName);
		// �����Ƿ�ִ����Ϊ�ض�����ϵͳ������������̨����
		void SetExecuteConsoleCommands(bool enable);
		// ���������������һ������������ֱ�������˳��롣����κ������·�������彫ʧ�ܡ�
		int SystemCommand(const String& commandLine, bool redirectStdOutToLog = false);
		// ������һ������������ֱ�������˳��롣����κ������·�������彫ʧ�ܡ�
		int SystemRun(const String& fileName, const Vector<String>& arguments);
		//������������첽����һ������ ����һ��request ID �� ���ʧ�ܷ���M_MAX_UNSIGNED�� ����˳��뽫��ͬ��һ��AsyncExecFinished�¼���request ID����������κ������·�������彫ʧ�ܡ�
		unsigned SystemCommandAsync(const String& commandLine);
		//�첽����һ������ ����һ��request ID �� ���ʧ�ܷ���M_MAX_UNSIGNED�� ����˳��뽫��ͬ��һ��AsyncExecFinished�¼���request ID����������κ������·�������彫ʧ�ܡ�
		unsigned SystemRunAsync(const String& fileName, const Vector<String>& arguments);
		/// ��ָ����ģʽ��edit����һ���ⲿ�����һ���ļ�������κ������·�������彫ʧ�ܡ�
		bool SystemOpen(const String& fileName, const String& mode = String::EMPTY);
		/// ����һ���ļ�������ɹ�����true
		bool Copy(const String& srcFileName, const String& destFileName);
		/// �������ļ�
		bool Rename(const String& srcFileName, const String& destFileName);
		/// ɾ���ļ�
		bool Delete(const String& fileName);
		/// ע��һ��������ʵ�·�������û��·����ע�ᣬ�����ж��ɱ�����
		void RegisterPath(const String& pathName);
		/// �����ļ���һ���޸�ʱ�� 
		bool SetLastModifiedTime(const String& fileName, unsigned newTime);

		///���ص�ǰ����·��
		String GetCurrentDir() const;

		/// ���������Ƿ�ִ�п���̨����
		bool GetExecuteConsoleCommands() const { return executeConsoleCommands_; }

		/// ·���Ƿ��Ѿ���ע����
		bool HasRegisteredPaths() const { return allowedPaths_.Size() > 0; }

		/// ���·���������
		bool CheckAccess(const String& pathName) const;
		/// �����ļ���1.1.1970�����һ���޸ĵ�ʱ�䣬 ���Ϊ0���޷�����
		unsigned GetLastModifiedTime(const String& fileName) const;
		/// �ļ��Ƿ����
		bool FileExists(const String& fileName) const;
		/// Ŀ¼�Ƿ����
		bool DirExists(const String& pathName) const;
		/// ɨ��Ŀ¼
		void ScanDir(Vector<String>& result, const String& pathName, const String& filter, unsigned flags, bool recursive) const;
		/// ���س����Ŀ¼��
		String GetProgramDir() const;
		/// Return the user documents directory.
		String GetUserDocumentsDir() const;
		/// Return the application preferences directory.
		String GetAppPreferencesDir(const String& org, const String& app) const;

	private:
		/// ɨ��Ŀ¼���ڲ�����
		void ScanDirInternal
			(Vector<String>& result, String path, const String& startPath, const String& filter, unsigned flags, bool recursive) const;
		/// ����BeginFrame�¼����������ɵ��첽ִ��
		void HandleBeginFrame(StringHash eventType, VariantMap& eventData);
		/// ����һ������̨������¼�
		void HandleConsoleCommand(StringHash eventType, VariantMap& eventData);

		/// ������ʵ�Ŀ¼
		HashSet<String> allowedPaths_;
		/// ����Ŀ¼�Ļ���
		mutable String programDir_;
		/// �첽ִ������
		List<AsyncExecRequest*> asyncExecQueue_;
		/// ��һ���첽��ִ��ID
		unsigned nextAsyncExecID_;
		/// ��Ϸ����ִ̨�в���ϵͳ����ı�־
		bool executeConsoleCommands_;
	};

	/// Split a full path to path, filename and extension. The extension will be converted to lowercase by default.
	SAPPHIRE_API void
		SplitPath(const String& fullPath, String& pathName, String& fileName, String& extension, bool lowercaseExtension = true);
	/// Return the path from a full path.
	SAPPHIRE_API String GetPath(const String& fullPath);
	/// Return the filename from a full path.
	SAPPHIRE_API String GetFileName(const String& fullPath);
	/// ����һ������·������չ����Ĭ��תΪСд
	SAPPHIRE_API String GetExtension(const String& fullPath, bool lowercaseExtension = true);
	/// Return the filename and extension from a full path. The case of the extension is preserved by default, so that the file can be opened in case-sensitive operating systems.
	SAPPHIRE_API String GetFileNameAndExtension(const String& fullPath, bool lowercaseExtension = false);
	/// Replace the extension of a file name with another.
	SAPPHIRE_API String ReplaceExtension(const String& fullPath, const String& newExtension);
	/// Add a slash at the end of the path if missing and convert to internal format (use slashes.)
	SAPPHIRE_API String AddTrailingSlash(const String& pathName);
	/// Remove the slash from the end of a path if exists and convert to internal format (use slashes.)
	SAPPHIRE_API String RemoveTrailingSlash(const String& pathName);
	/// Return the parent path, or the path itself if not available.
	SAPPHIRE_API String GetParentPath(const String& pathName);
	/// Convert a path to internal format (use slashes.)
	SAPPHIRE_API String GetInternalPath(const String& pathName);
	/// Convert a path to the format required by the operating system.
	SAPPHIRE_API String GetNativePath(const String& pathName);
	/// Convert a path to the format required by the operating system in wide characters.
	SAPPHIRE_API WString GetWideNativePath(const String& pathName);
	/// Return whether a path is absolute.
	SAPPHIRE_API bool IsAbsolutePath(const String& pathName);
}
