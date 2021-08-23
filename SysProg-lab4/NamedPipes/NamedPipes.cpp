// AnonymousDll.cpp: определяет процедуры инициализации для библиотеки DLL.
//
#include "pch.h"
#include "framework.h"
#include "NamedPipes.h"
#include <string>
#include "..\\lab4cpp\\lab4cpp.h"

#define PIPE_NAME (LPCSTR)"\\\\.\\pipe\\YeahPipe"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CNamedPipesApp, CWinApp)
END_MESSAGE_MAP()

void ErrorMessage() 
{
	CString s = "No pipes available";
	AfxMessageBox(s);
}


CNamedPipesApp::CNamedPipesApp()
{
}


CNamedPipesApp theApp;


extern "C" 
{
	
	__declspec(dllexport) int GetAmount(int evType)
	{
		int amount = 0;
		if (WaitNamedPipe(PIPE_NAME, 5000))
		{
			HANDLE hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			DWORD dwWrite, dwRead;
			WriteFile(hPipe, LPCVOID(&evType), sizeof(evType), &dwWrite, NULL);
			ReadFile(hPipe, LPVOID(&amount), sizeof(amount), &dwRead, NULL);

			CloseHandle(hPipe);

			return amount;
		}
		else
			return 0;

	}

	__declspec(dllexport) bool StartThread(int evType)
	{
		if (WaitNamedPipe(PIPE_NAME, 5000))
		{

			HANDLE hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			DWORD dwWrite;
			WriteFile(hPipe, LPCVOID(&evType), sizeof(evType), &dwWrite, NULL);
			CloseHandle(hPipe);
			return true;
		}
		else
		{
			return false;
			//ErrorMessage();
		}
	}

	__declspec(dllexport) bool StopThread(int evType)
	{
		if (WaitNamedPipe(PIPE_NAME, 5000))
		{
			HANDLE hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			DWORD dwWrite;

			WriteFile(hPipe, LPCVOID(&evType), sizeof(evType), &dwWrite, NULL);

			CloseHandle(hPipe);
			return true;
		}
		else
		{
			return false;
			//ErrorMessage();
		}
	}


	 _declspec(dllexport) void __stdcall Send(int evType, int thId, const char* msg) 
	 {
		if (WaitNamedPipe(PIPE_NAME, 5000))
		{
			int strSize = strlen(msg) + 1;
			HANDLE hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			DWORD dwWrite;			
			WriteFile(hPipe, LPCVOID(&evType), sizeof(evType), &dwWrite, NULL);
			WriteFile(hPipe, LPCVOID(&thId), sizeof(thId), &dwWrite, NULL); 
			WriteFile(hPipe, LPCVOID(msg), strlen(msg), &dwWrite, NULL);
			CloseHandle(hPipe);
		}
		else
		{
			ErrorMessage();
		}
	}
}

BOOL CNamedPipesApp::InitInstance()
{
	CWinApp::InitInstance();
	return TRUE;
}
