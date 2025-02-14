#include "Predefined.h"

#include "../Container/ArrayPtr.h"
#include "../Core/Context.h"
#include "../Core/CoreEvents.h"
#include "../Core/Thread.h"
#include "EngineEvents.h"
#include "File.h"
#include "FileSystem.h"
#include "IOEvents.h"
#include "Log.h"
//使用SDL文件系统
#include <SDL/include/SDL_filesystem.h>

#include <sys/stat.h>

#ifdef WIN32
#include <cstdio>
#ifndef _MSC_VER
#define _WIN32_IE 0x501
#endif
#include <windows.h>
#include <shellapi.h>
#include <direct.h>
#include <shlobj.h>
#include <sys/types.h>
#include <sys/utime.h>
#else
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <utime.h>
#include <sys/wait.h>
#define MAX_PATH 256
#endif

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

extern "C"
{
#ifdef ANDROID
	const char* SDL_Android_GetFilesDir();
	char** SDL_Android_GetFileList(const char* path, int* count);
	void SDL_Android_FreeFileList(char*** array, int* count);
#elif IOS
	const char* SDL_IOS_GetResourceDir();
	const char* SDL_IOS_GetDocumentsDir();
#endif
}

#include "DebugNew.h"


namespace Sapphire
{

	//执行系统命令
	int DoSystemCommand(const String& commandLine, bool redirectToLog, Context* context)
	{
		//没有popen函数
#if !defined(NO_POPEN) 
		if (!redirectToLog)   //不重定向log
#endif
			//执行控制台命令
			return system(commandLine.CString());

#if !defined(NO_POPEN)
		//获取一个平台无关的临时文件名作为stderr重定向
		String stderrFilename;
		String adjustedCommandLine(commandLine);
		char* prefPath = SDL_GetPrefPath("SAPPHIRE", "temp");
		if (prefPath)
		{
			stderrFilename = String(prefPath) + "command-stderr";
			adjustedCommandLine += " 2>" + stderrFilename;
			SDL_free(prefPath);
		}

#ifdef _MSC_VER
#define popen _popen
#define pclose _pclose
#endif

		//使用popen/pcloase来捕获控制台的stdout和stderr
		FILE* file = popen(adjustedCommandLine.CString(), "r");
		if (!file)
			return -1;

		// 捕获标准输出流
		char buffer[128];
		while (!feof(file))
		{
			if (fgets(buffer, sizeof(buffer), file))
				SAPPHIRE_LOGRAW(String(buffer));
		}
		int exitCode = pclose(file);

		// 捕获标准错误流
		if (!stderrFilename.Empty())
		{
			//创建一个错误文件
			SharedPtr<File> errFile(new File(context, stderrFilename, FILE_READ));
			while (!errFile->IsEof())
			{
				//读取内容
				unsigned numRead = errFile->Read(buffer, sizeof(buffer));
				if (numRead)
					Log::WriteRaw(String(buffer, numRead), true);
			}
		}

		return exitCode;
#endif
	}

	int DoSystemRun(const String& fileName, const Vector<String>& arguments)
	{
		String fixedFileName = GetNativePath(fileName);

#ifdef WIN32
		// 如果扩展名不存在，添加.exe扩展名
		if (GetExtension(fixedFileName).Empty())
			fixedFileName += ".exe";

		String commandLine = "\"" + fixedFileName + "\"";
		for (unsigned i = 0; i < arguments.Size(); ++i)
			commandLine += " " + arguments[i];

		//创建子进程
		STARTUPINFOW startupInfo;
		PROCESS_INFORMATION processInfo;
		memset(&startupInfo, 0, sizeof startupInfo);
		memset(&processInfo, 0, sizeof processInfo);

		WString commandLineW(commandLine);
		if (!CreateProcessW(NULL, (wchar_t*)commandLineW.CString(), 0, 0, 0, CREATE_NO_WINDOW, 0, 0, &startupInfo, &processInfo))
			return -1;
		//等待进程返回
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		DWORD exitCode;
		GetExitCodeProcess(processInfo.hProcess, &exitCode);

		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);

		return exitCode;
#else    //Linux
		pid_t pid = fork();
		if (!pid)
		{
			PODVector<const char*> argPtrs;
			argPtrs.Push(fixedFileName.CString());
			for (unsigned i = 0; i < arguments.Size(); ++i)
				argPtrs.Push(arguments[i].CString());
			argPtrs.Push(0);

			execvp(argPtrs[0], (char**)&argPtrs[0]);
			return -1; // Return -1 不能创建进程
		}
		else if (pid > 0)
		{
			int exitCode;
			wait(&exitCode);
			return exitCode;
		}
		else
			return -1;
#endif
	}

	//对于异步执行请求的基类
	class AsyncExecRequest : public Thread
	{
	public:
		//请求ID
		AsyncExecRequest(unsigned& requestID) :
			requestID_(requestID),
			completed_(false)
		{
			// 为下次请求增加ＩＤ
			++requestID;
			if (requestID == M_MAX_UNSIGNED)
				requestID = 1;
		}
		// 返回请求ID
		unsigned GetRequestID() const { return requestID_; }

		//返回退出码，当IsCompleted（）为true时有效
		int GetExitCode() const { return exitCode_; }

		/// 返回完成状态
		bool IsCompleted() const { return completed_; }

	protected:
		/// 请求ID
		unsigned requestID_;
		/// Exit code.
		int exitCode_;
		/// 完成标志.
		volatile bool completed_;
	};

	/// 异步系统命令操作
	class AsyncSystemCommand : public AsyncExecRequest
	{
	public:
		/// 构造并允许
		AsyncSystemCommand(unsigned requestID, const String& commandLine) :
			AsyncExecRequest(requestID),
			commandLine_(commandLine)
		{
			Run();
		}

		/// 这个函数在线程中运行
		virtual void ThreadFunction()
		{
			exitCode_ = DoSystemCommand(commandLine_, false, 0);
			completed_ = true;
		}

	private:
		/// 命令行
		String commandLine_;
	};


	/// 异步系统运行操作
	class AsyncSystemRun : public AsyncExecRequest
	{
	public:
		/// 构造并运行
		AsyncSystemRun(unsigned requestID, const String& fileName, const Vector<String>& arguments) :
			AsyncExecRequest(requestID),
			fileName_(fileName),
			arguments_(arguments)
		{
			Run();
		}

	
		virtual void ThreadFunction()
		{
			exitCode_ = DoSystemRun(fileName_, arguments_);
			completed_ = true;
		}

	private:
		/// 运行的文件名
		String fileName_;
		///命令行分割的参数
		const Vector<String>& arguments_;
	};

	FileSystem::FileSystem(Context* context) :
		Object(context),
		nextAsyncExecID_(1),
		executeConsoleCommands_(false)
	{
		SubscribeToEvent(E_BEGINFRAME, SAPPHIRE_HANDLER(FileSystem, HandleBeginFrame));

		// 订阅到控制台命令
		SetExecuteConsoleCommands(true);
	}

	FileSystem::~FileSystem()
	{
		 
		if (asyncExecQueue_.Size())
		{
			for (List<AsyncExecRequest*>::Iterator i = asyncExecQueue_.Begin(); i != asyncExecQueue_.End(); ++i)
				delete(*i);

			asyncExecQueue_.Clear();
		}
	}

	bool FileSystem::SetCurrentDir(const String& pathName)
	{
		//检查合法性
		if (!CheckAccess(pathName))
		{
			SAPPHIRE_LOGERROR("Access denied to " + pathName);
			return false;
		}
#ifdef WIN32
		if (SetCurrentDirectoryW(GetWideNativePath(pathName).CString()) == FALSE)
		{
			SAPPHIRE_LOGERROR("Failed to change directory to " + pathName);
			return false;
		}
#else
		if (chdir(GetNativePath(pathName).CString()) != 0)
		{
			SAPPHIRE_LOGERROR("Failed to change directory to " + pathName);
			return false;
		}
#endif

		return true;
	}

	bool FileSystem::CreateDir(const String& pathName)
	{
		if (!CheckAccess(pathName))
		{
			SAPPHIRE_LOGERROR("Access denied to " + pathName);
			return false;
		}

		// Create each of the parents if necessary
		String parentPath = GetParentPath(pathName);
		if (parentPath.Length() > 1 && !DirExists(parentPath))
		{
			if (!CreateDir(parentPath))
				return false;
		}

#ifdef WIN32
		bool success = (CreateDirectoryW(GetWideNativePath(RemoveTrailingSlash(pathName)).CString(), 0) == TRUE) ||
			(GetLastError() == ERROR_ALREADY_EXISTS);
#else
		bool success = mkdir(GetNativePath(RemoveTrailingSlash(pathName)).CString(), S_IRWXU) == 0 || errno == EEXIST;
#endif

		if (success)
			SAPPHIRE_LOGDEBUG("Created directory " + pathName);
		else
			SAPPHIRE_LOGERROR("Failed to create directory " + pathName);

		return success;
	}

	void FileSystem::SetExecuteConsoleCommands(bool enable)
	{
		if (enable == executeConsoleCommands_)
			return;

		executeConsoleCommands_ = enable;
		//执行控制台命令
		if (enable)
			SubscribeToEvent(E_CONSOLECOMMAND, SAPPHIRE_HANDLER(FileSystem, HandleConsoleCommand));
		else
			UnsubscribeFromEvent(E_CONSOLECOMMAND);
	}

	int FileSystem::SystemCommand(const String& commandLine, bool redirectStdOutToLog)
	{
		if (allowedPaths_.Empty())
			return DoSystemCommand(commandLine, redirectStdOutToLog, context_);
		else
		{
			SAPPHIRE_LOGERROR("Executing an external command is not allowed");
			return -1;
		}
	}

	int FileSystem::SystemRun(const String& fileName, const Vector<String>& arguments)
	{
		if (allowedPaths_.Empty())
			return DoSystemRun(fileName, arguments);
		else
		{
			SAPPHIRE_LOGERROR("Executing an external command is not allowed");
			return -1;
		}
	}

	unsigned FileSystem::SystemCommandAsync(const String& commandLine)
	{
#ifdef SAPPHIRE_THREADING
		if (allowedPaths_.Empty())
		{
			unsigned requestID = nextAsyncExecID_;
			AsyncSystemCommand* cmd = new AsyncSystemCommand(nextAsyncExecID_, commandLine);
			asyncExecQueue_.Push(cmd);
			return requestID;
		}
		else
		{
			SAPPHIRE_LOGERROR("Executing an external command is not allowed");
			return M_MAX_UNSIGNED;
		}
#else
		SAPPHIRE_LOGERROR("Can not execute an asynchronous command as threading is disabled");
		return M_MAX_UNSIGNED;
#endif
	}

	unsigned FileSystem::SystemRunAsync(const String& fileName, const Vector<String>& arguments)
	{
#ifdef SAPPHIRE_THREADING
		if (allowedPaths_.Empty())
		{
			unsigned requestID = nextAsyncExecID_;   //分配当前执行的异步执行ID
			AsyncSystemRun* cmd = new AsyncSystemRun(nextAsyncExecID_, fileName, arguments);
			asyncExecQueue_.Push(cmd);  //推入异步执行队列中
			return requestID;
		}
		else
		{
			SAPPHIRE_LOGERROR("Executing an external command is not allowed");
			return M_MAX_UNSIGNED;
		}
#else
		SAPPHIRE_LOGERROR("Can not run asynchronously as threading is disabled");
		return M_MAX_UNSIGNED;
#endif
	}

	bool FileSystem::SystemOpen(const String& fileName, const String& mode)
	{
		if (allowedPaths_.Empty())
		{
			if (!FileExists(fileName) && !DirExists(fileName))
			{
				SAPPHIRE_LOGERROR("File or directory " + fileName + " not found");
				return false;
			}

#ifdef WIN32
			bool success = (size_t)ShellExecuteW(0, !mode.Empty() ? WString(mode).CString() : 0,
				GetWideNativePath(fileName).CString(), 0, 0, SW_SHOW) > 32;
#else
			Vector<String> arguments;
			arguments.Push(fileName);
			bool success = SystemRun(
#if defined(__APPLE__)
				"/usr/bin/open",
#else
				"/usr/bin/xdg-open",
#endif
				arguments) == 0;
#endif
			if (!success)
				SAPPHIRE_LOGERROR("Failed to open " + fileName + " externally");
			return success;
		}
		else
		{
			SAPPHIRE_LOGERROR("Opening a file externally is not allowed");
			return false;
		}
	}

	bool FileSystem::Copy(const String& srcFileName, const String& destFileName)
	{
		if (!CheckAccess(GetPath(srcFileName)))
		{
			SAPPHIRE_LOGERROR("Access denied to " + srcFileName);
			return false;
		}
		if (!CheckAccess(GetPath(destFileName)))
		{
			SAPPHIRE_LOGERROR("Access denied to " + destFileName);
			return false;
		}

		SharedPtr<File> srcFile(new File(context_, srcFileName, FILE_READ));
		if (!srcFile->IsOpen())
			return false;
		SharedPtr<File> destFile(new File(context_, destFileName, FILE_WRITE));
		if (!destFile->IsOpen())
			return false;

		unsigned fileSize = srcFile->GetSize();
		SharedArrayPtr<unsigned char> buffer(new unsigned char[fileSize]);

		unsigned bytesRead = srcFile->Read(buffer.Get(), fileSize);
		unsigned bytesWritten = destFile->Write(buffer.Get(), fileSize);
		return bytesRead == fileSize && bytesWritten == fileSize;
	}

	bool FileSystem::Rename(const String& srcFileName, const String& destFileName)
	{
		if (!CheckAccess(GetPath(srcFileName)))
		{
			SAPPHIRE_LOGERROR("Access denied to " + srcFileName);
			return false;
		}
		if (!CheckAccess(GetPath(destFileName)))
		{
			SAPPHIRE_LOGERROR("Access denied to " + destFileName);
			return false;
		}

#ifdef WIN32
		return MoveFileW(GetWideNativePath(srcFileName).CString(), GetWideNativePath(destFileName).CString()) != 0;
#else
		return rename(GetNativePath(srcFileName).CString(), GetNativePath(destFileName).CString()) == 0;
#endif
	}

	bool FileSystem::Delete(const String& fileName)
	{
		if (!CheckAccess(GetPath(fileName)))
		{
			SAPPHIRE_LOGERROR("Access denied to " + fileName);
			return false;
		}

#ifdef WIN32
		return DeleteFileW(GetWideNativePath(fileName).CString()) != 0;
#else
		return remove(GetNativePath(fileName).CString()) == 0;
#endif
	}

	String FileSystem::GetCurrentDir() const
	{
#ifdef WIN32
		wchar_t path[MAX_PATH];
		path[0] = 0;
		GetCurrentDirectoryW(MAX_PATH, path);  //获取当前进程的目录
		return AddTrailingSlash(String(path));
#else
		char path[MAX_PATH];
		path[0] = 0;
		getcwd(path, MAX_PATH);
		return AddTrailingSlash(String(path));
#endif
	}

	bool FileSystem::CheckAccess(const String& pathName) const
	{
		String fixedPath = AddTrailingSlash(pathName);

		// 如果没有允许的路径定义，返回true
		if (allowedPaths_.Empty())
			return true;

		// 不允许访问父目录
		if (fixedPath.Contains(".."))
			return false;

		// Check if the path is a partial match of any of the allowed directories
		for (HashSet<String>::ConstIterator i = allowedPaths_.Begin(); i != allowedPaths_.End(); ++i)
		{
			if (fixedPath.Find(*i) == 0)
				return true;
		}

		// 没找到
		return false;
	}

	//获取上次修改时间
	unsigned FileSystem::GetLastModifiedTime(const String& fileName) const
	{
		if (fileName.Empty() || !CheckAccess(fileName))
			return 0;

#ifdef WIN32
		struct _stat st;
		if (!_stat(fileName.CString(), &st))
			return (unsigned)st.st_mtime;
		else
			return 0;
#else
		struct stat st;
		if (!stat(fileName.CString(), &st))
			return (unsigned)st.st_mtime;
		else
			return 0;
#endif
	}

	bool FileSystem::FileExists(const String& fileName) const
	{
		if (!CheckAccess(GetPath(fileName)))
			return false;

#ifdef ANDROID
		if (SAPPHIRE_IS_ASSET(fileName))
		{
			SDL_RWops* rwOps = SDL_RWFromFile(SAPPHIRE_ASSET(fileName), "rb");
			if (rwOps)
			{
				SDL_RWclose(rwOps);
				return true;
			}
			else
				return false;
		}
#endif

		String fixedName = GetNativePath(RemoveTrailingSlash(fileName));

#ifdef WIN32
		//获取文件属性
		DWORD attributes = GetFileAttributesW(WString(fixedName).CString());
		if (attributes == INVALID_FILE_ATTRIBUTES || attributes & FILE_ATTRIBUTE_DIRECTORY)
			return false;
#else
		struct stat st;
		if (stat(fixedName.CString(), &st) || st.st_mode & S_IFDIR)
			return false;
#endif

		return true;
	}

	bool FileSystem::DirExists(const String& pathName) const
	{
		if (!CheckAccess(pathName))
			return false;

#ifndef WIN32
		// Always return true for the root directory
		if (pathName == "/")
			return true;
#endif

		String fixedName = GetNativePath(RemoveTrailingSlash(pathName));

#ifdef ANDROID
		if (SAPPHIRE_IS_ASSET(fixedName))
		{
			// Split the pathname into two components: the longest parent directory path and the last name component
			String assetPath(SAPPHIRE_ASSET((fixedName + '/')));
			String parentPath;
			unsigned pos = assetPath.FindLast('/', assetPath.Length() - 2);
			if (pos != String::NPOS)
			{
				parentPath = assetPath.Substring(0, pos - 1);
				assetPath = assetPath.Substring(pos + 1);
			}
			assetPath.Resize(assetPath.Length() - 1);

			bool exist = false;
			int count;
			char** list = SDL_Android_GetFileList(parentPath.CString(), &count);
			for (int i = 0; i < count; ++i)
			{
				exist = assetPath == list[i];
				if (exist)
					break;
			}
			SDL_Android_FreeFileList(&list, &count);
			return exist;
		}
#endif

#ifdef WIN32
		DWORD attributes = GetFileAttributesW(WString(fixedName).CString());
		if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY))
			return false;
#else
		struct stat st;
		if (stat(fixedName.CString(), &st) || !(st.st_mode & S_IFDIR))
			return false;
#endif

		return true;
	}

	void FileSystem::ScanDir(Vector<String>& result, const String& pathName, const String& filter, unsigned flags, bool recursive) const
	{
		result.Clear();

		if (CheckAccess(pathName))
		{
			String initialPath = AddTrailingSlash(pathName);
			ScanDirInternal(result, initialPath, initialPath, filter, flags, recursive);
		}
	}

	String FileSystem::GetProgramDir() const
	{
		// 如果可能，返回缓存值
		if (!programDir_.Empty())
			return programDir_;

#if defined(ANDROID)
		// This is an internal directory specifier pointing to the assets in the .apk
		// Files from this directory will be opened using special handling
		programDir_ = APK;
		return programDir_;
#elif defined(IOS)
		programDir_ = AddTrailingSlash(SDL_IOS_GetResourceDir());
		return programDir_;
#elif defined(WIN32)
		wchar_t exeName[MAX_PATH];
		exeName[0] = 0;
		GetModuleFileNameW(0, exeName, MAX_PATH);  //获取程序文件所在目录
		programDir_ = GetPath(String(exeName));
#elif defined(__APPLE__)
		char exeName[MAX_PATH];
		memset(exeName, 0, MAX_PATH);
		unsigned size = MAX_PATH;
		_NSGetExecutablePath(exeName, &size);
		programDir_ = GetPath(String(exeName));
#elif defined(__linux__)
		char exeName[MAX_PATH];
		memset(exeName, 0, MAX_PATH);
		pid_t pid = getpid();
		String link = "/proc/" + String(pid) + "/exe";
		readlink(link.CString(), exeName, MAX_PATH);
		programDir_ = GetPath(String(exeName));
#endif

		// If the executable directory does not contain CoreData & Data directories, but the current working directory does, use the
		// current working directory instead
		/// \todo Should not rely on such fixed convention
		String currentDir = GetCurrentDir();
		if (!DirExists(programDir_ + "CoreData") && !DirExists(programDir_ + "Data") &&
			(DirExists(currentDir + "CoreData") || DirExists(currentDir + "Data")))
			programDir_ = currentDir;

		// Sanitate /./ construct away
		programDir_.Replace("/./", "/");

		return programDir_;
	}

	String FileSystem::GetUserDocumentsDir() const
	{
#if defined(ANDROID)
		return AddTrailingSlash(SDL_Android_GetFilesDir());
#elif defined(IOS)
		return AddTrailingSlash(SDL_IOS_GetDocumentsDir());
#elif defined(WIN32)
		wchar_t pathName[MAX_PATH];
		pathName[0] = 0;
		SHGetSpecialFolderPathW(0, pathName, CSIDL_PERSONAL, 0);
		return AddTrailingSlash(String(pathName));
#else
		char pathName[MAX_PATH];
		pathName[0] = 0;
		strcpy(pathName, getenv("HOME"));
		return AddTrailingSlash(String(pathName));
#endif
	}

	String FileSystem::GetAppPreferencesDir(const String& org, const String& app) const
	{
		String dir;
		char* prefPath = SDL_GetPrefPath(org.CString(), app.CString());
		if (prefPath)
		{
			dir = GetInternalPath(String(prefPath));
			SDL_free(prefPath);
		}
		else
			SAPPHIRE_LOGWARNING("Could not get application preferences directory");

		return dir;
	}

	void FileSystem::RegisterPath(const String& pathName)
	{
		if (pathName.Empty())
			return;

		allowedPaths_.Insert(AddTrailingSlash(pathName));
	}

	bool FileSystem::SetLastModifiedTime(const String& fileName, unsigned newTime)
	{
		if (fileName.Empty() || !CheckAccess(fileName))
			return false;

#ifdef WIN32
		struct _stat oldTime;
		struct _utimbuf newTimes;
		if (_stat(fileName.CString(), &oldTime) != 0)
			return false;
		newTimes.actime = oldTime.st_atime;
		newTimes.modtime = newTime;
		return _utime(fileName.CString(), &newTimes) == 0;
#else
		struct stat oldTime;
		struct utimbuf newTimes;
		if (stat(fileName.CString(), &oldTime) != 0)
			return false;
		newTimes.actime = oldTime.st_atime;
		newTimes.modtime = newTime;
		return utime(fileName.CString(), &newTimes) == 0;
#endif
	}

	void FileSystem::ScanDirInternal(Vector<String>& result, String path, const String& startPath,
		const String& filter, unsigned flags, bool recursive) const
	{
		path = AddTrailingSlash(path);
		String deltaPath;
		if (path.Length() > startPath.Length())
			deltaPath = path.Substring(startPath.Length());

		String filterExtension = filter.Substring(filter.Find('.'));
		if (filterExtension.Contains('*'))
			filterExtension.Clear();

#ifdef ANDROID
		if (SAPPHIRE_IS_ASSET(path))
		{
			String assetPath(SAPPHIRE_ASSET(path));
			assetPath.Resize(assetPath.Length() - 1);       // AssetManager.list() does not like trailing slash
			int count;
			char** list = SDL_Android_GetFileList(assetPath.CString(), &count);
			for (int i = 0; i < count; ++i)
			{
				String fileName(list[i]);
				if (!(flags & SCAN_HIDDEN) && fileName.StartsWith("."))
					continue;

#ifdef ASSET_DIR_INDICATOR
				// Patch the directory name back after retrieving the directory flag
				bool isDirectory = fileName.EndsWith(ASSET_DIR_INDICATOR);
				if (isDirectory)
				{
					fileName.Resize(fileName.Length() - sizeof(ASSET_DIR_INDICATOR) / sizeof(char) + 1);
					if (flags & SCAN_DIRS)
						result.Push(deltaPath + fileName);
					if (recursive)
						ScanDirInternal(result, path + fileName, startPath, filter, flags, recursive);
				}
				else if (flags & SCAN_FILES)
#endif
				{
					if (filterExtension.Empty() || fileName.EndsWith(filterExtension))
						result.Push(deltaPath + fileName);
				}
			}
			SDL_Android_FreeFileList(&list, &count);
			return;
		}
#endif
#ifdef WIN32
		WIN32_FIND_DATAW info;
		HANDLE handle = FindFirstFileW(WString(path + "*").CString(), &info);
		if (handle != INVALID_HANDLE_VALUE)
		{
			do
			{
				String fileName(info.cFileName);
				if (!fileName.Empty())
				{
					if (info.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN && !(flags & SCAN_HIDDEN))
						continue;
					if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (flags & SCAN_DIRS)
							result.Push(deltaPath + fileName);
						if (recursive && fileName != "." && fileName != "..")
							ScanDirInternal(result, path + fileName, startPath, filter, flags, recursive);
					}
					else if (flags & SCAN_FILES)
					{
						if (filterExtension.Empty() || fileName.EndsWith(filterExtension))
							result.Push(deltaPath + fileName);
					}
				}
			} while (FindNextFileW(handle, &info));

			FindClose(handle);
		}
#else
		DIR* dir;
		struct dirent* de;
		struct stat st;
		dir = opendir(GetNativePath(path).CString());
		if (dir)
		{
			while ((de = readdir(dir)))
			{
				/// \todo Filename may be unnormalized Unicode on Mac OS X. Re-normalize as necessary
				String fileName(de->d_name);
				bool normalEntry = fileName != "." && fileName != "..";
				if (normalEntry && !(flags & SCAN_HIDDEN) && fileName.StartsWith("."))
					continue;
				String pathAndName = path + fileName;
				if (!stat(pathAndName.CString(), &st))
				{
					if (st.st_mode & S_IFDIR)
					{
						if (flags & SCAN_DIRS)
							result.Push(deltaPath + fileName);
						if (recursive && normalEntry)
							ScanDirInternal(result, path + fileName, startPath, filter, flags, recursive);
					}
					else if (flags & SCAN_FILES)
					{
						if (filterExtension.Empty() || fileName.EndsWith(filterExtension))
							result.Push(deltaPath + fileName);
					}
				}
			}
			closedir(dir);
		}
#endif
	}

	void FileSystem::HandleBeginFrame(StringHash eventType, VariantMap& eventData)
	{
		// 遍历执行队列并且抛出移除完成的异步请求
		for (List<AsyncExecRequest*>::Iterator i = asyncExecQueue_.Begin(); i != asyncExecQueue_.End();)
		{
			AsyncExecRequest* request = *i;
			if (request->IsCompleted())
			{
				using namespace AsyncExecFinished;
				//获取事件数据MAP
				VariantMap& newEventData = GetEventDataMap();
				//设置该已完成请求的ID
				newEventData[P_REQUESTID] = request->GetRequestID();
				//设置该请求ExitCode
				newEventData[P_EXITCODE] = request->GetExitCode();
				//发送异步执行完成事件
				SendEvent(E_ASYNCEXECFINISHED, newEventData);
				//执行完毕，从异步队列中清除
				delete request;
				i = asyncExecQueue_.Erase(i);
			}
			else
				++i;
		}
	}

	void FileSystem::HandleConsoleCommand(StringHash eventType, VariantMap& eventData)
	{
		using namespace ConsoleCommand;
		if (eventData[P_ID].GetString() == GetTypeName())
			SystemCommand(eventData[P_COMMAND].GetString(), true);
	}

	void SplitPath(const String& fullPath, String& pathName, String& fileName, String& extension, bool lowercaseExtension)
	{
		String fullPathCopy = GetInternalPath(fullPath);

		unsigned extPos = fullPathCopy.FindLast('.');
		unsigned pathPos = fullPathCopy.FindLast('/');

		if (extPos != String::NPOS && (pathPos == String::NPOS || extPos > pathPos))
		{
			extension = fullPathCopy.Substring(extPos);
			if (lowercaseExtension)
				extension = extension.ToLower();
			fullPathCopy = fullPathCopy.Substring(0, extPos);
		}
		else
			extension.Clear();

		pathPos = fullPathCopy.FindLast('/');
		if (pathPos != String::NPOS)
		{
			fileName = fullPathCopy.Substring(pathPos + 1);
			pathName = fullPathCopy.Substring(0, pathPos + 1);
		}
		else
		{
			fileName = fullPathCopy;
			pathName.Clear();
		}
	}

	String GetPath(const String& fullPath)
	{
		String path, file, extension;
		SplitPath(fullPath, path, file, extension);
		return path;
	}

	String GetFileName(const String& fullPath)
	{
		String path, file, extension;
		SplitPath(fullPath, path, file, extension);
		return file;
	}

	String GetExtension(const String& fullPath, bool lowercaseExtension)
	{
		String path, file, extension;
		SplitPath(fullPath, path, file, extension, lowercaseExtension);
		return extension;
	}

	String GetFileNameAndExtension(const String& fileName, bool lowercaseExtension)
	{
		String path, file, extension;
		SplitPath(fileName, path, file, extension, lowercaseExtension);
		return file + extension;
	}

	String ReplaceExtension(const String& fullPath, const String& newExtension)
	{
		String path, file, extension;
		SplitPath(fullPath, path, file, extension);
		return path + file + newExtension;
	}

	String AddTrailingSlash(const String& pathName)
	{
		String ret = pathName.Trimmed();
		ret.Replace('\\', '/');
		if (!ret.Empty() && ret.Back() != '/')
			ret += '/';
		return ret;
	}

	String RemoveTrailingSlash(const String& pathName)
	{
		String ret = pathName.Trimmed();
		ret.Replace('\\', '/');
		if (!ret.Empty() && ret.Back() == '/')
			ret.Resize(ret.Length() - 1);
		return ret;
	}

	String GetParentPath(const String& path)
	{
		unsigned pos = RemoveTrailingSlash(path).FindLast('/');
		if (pos != String::NPOS)
			return path.Substring(0, pos + 1);
		else
			return String();
	}

	String GetInternalPath(const String& pathName)
	{
		return pathName.Replaced('\\', '/');
	}

	String GetNativePath(const String& pathName)
	{
#ifdef WIN32
		return pathName.Replaced('/', '\\');
#else
		return pathName;
#endif
	}

	WString GetWideNativePath(const String& pathName)
	{
#ifdef WIN32
		return WString(pathName.Replaced('/', '\\'));
#else
		return WString(pathName);
#endif
	}

	bool IsAbsolutePath(const String& pathName)
	{
		if (pathName.Empty())
			return false;

		String path = GetInternalPath(pathName);

		if (path[0] == '/')
			return true;

#ifdef WIN32
		if (path.Length() > 1 && IsAlpha(path[0]) && path[1] == ':')
			return true;
#endif

		return false;
	}
}