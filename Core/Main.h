#pragma once


#include "ProcessUtil.h"

#if defined(WIN32) && !defined(SAPPHIRE_WIN32_CONSOLE)
#include "../Core/MiniDump.h"
#include <windows.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif
#endif

//����һ��ƽ̨�ض���������

// MSVC debugģʽ��ʹ���ڴ�й¶����
#if defined(_MSC_VER) && defined(_DEBUG) && !defined(SAPPHIRE_WIN32_CONSOLE)
//WINMAIN��������
#define SAPPHIRE_DEFINE_MAIN(function) \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) \
{ \
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); \
    Sapphire::ParseArguments(GetCommandLineW()); \
    return function; \
}

// MSVC release ģʽ����crashʱд��minidump
#elif defined(_MSC_VER) && defined(SAPPHIRE_MINIDUMPS) && !defined(SAPPHIRE_WIN32_CONSOLE)
#define SAPPHIRE_DEFINE_MAIN(function) \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) \
{ \
    Sapphire::ParseArguments(GetCommandLineW()); \
    int exitCode; \
    __try \
	    { \
        exitCode = function; \
	    } \
    __except(Sapphire::WriteMiniDump("SAPPHIRE", GetExceptionInformation())) \
	    { \
	    } \
    return exitCode; \
}

//�����WIN32���߹ر�minidump
#elif defined(WIN32) && !defined(SAPPHIRE_WIN32_CONSOLE)
#define SAPPHIRE_DEFINE_MAIN(function) \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) \
{ \
    Sapphire::ParseArguments(GetCommandLineW()); \
    return function; \
}
// Android �� iOS: ʹ��SDL
#elif defined(ANDROID) || defined(IOS)
#define SAPPHIRE_DEFINE_MAIN(function) \
extern "C" int SDL_main(int argc, char** argv); \
int SDL_main(int argc, char** argv) \
{ \
    SAPPHIRE::ParseArguments(argc, argv); \
    return function; \
}
// ����
#else
#define SAPPHIRE_DEFINE_MAIN(function) \
int main(int argc, char** argv) \
{ \
    SAPPHIRE::ParseArguments(argc, argv); \
    return function; \
}
#endif