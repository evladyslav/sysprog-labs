// AnonymousDll.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "AnonymousDll.h"
#include <string>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CAnonymousDllApp, CWinApp)
END_MESSAGE_MAP()

CAnonymousDllApp::CAnonymousDllApp()
{	

}

CAnonymousDllApp theApp;

HANDLE hRead, hWrite;

void Cleanup()
{
	CloseHandle(hRead);
	CloseHandle(hWrite);
}

void LaunchClient()
{
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	CreateProcess(NULL, (LPSTR)"PipesClient.exe", NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

void ProcessClient(HANDLE hPipe)
{
	static int i = 0;
	while (true)
	{
		std::string s = GetString(hPipe);
		std::cout << s << std::endl;
		if (s == "quit")
		{
			break;
		}
		SendInt(hPipe, ++i);
	}
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
}


extern "C" _declspec(dllexport) void __stdcall StartChild()
{
	SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
	CreateNamedPipe( );

	SetHandleInformation(hWrite, HANDLE_FLAG_INHERIT, 0);
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = hRead;

	PROCESS_INFORMATION pi;
	CreateProcess(NULL, (LPSTR)"..\\..\\Debug\\lab2cpp.exe", &sa, NULL, TRUE, NULL, NULL, NULL, &si, &pi);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

extern "C" _declspec(dllexport) void __stdcall Send(const char* pStr)
{
	DWORD dwWrite;
	WriteFile(hWrite, pStr, strlen(pStr), &dwWrite, nullptr);
}

extern "C" _declspec(dllexport) int __stdcall KillChild()
{
	return 0;
}

BOOL CAnonymousDllApp::InitInstance()
{
	CWinApp::InitInstance();
	return TRUE;
}
