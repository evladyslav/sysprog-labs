// WindowsSocket.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "WindowsSocket.h"
#include <afxsock.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CWindowsSocketApp, CWinApp)
END_MESSAGE_MAP()


CWindowsSocketApp::CWindowsSocketApp()
{
}

CWindowsSocketApp theApp;

extern "C" 
{
	__declspec(dllexport) int SocketGetAmount(int evType)
	{
		int amount = NULL;
		AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
		AfxSocketInit();
		CSocket s;
		s.Create();

		if (s.Connect("127.0.0.1", 12345))
		{
			s.Send(LPCVOID(&evType), sizeof(evType));
			s.Receive(LPVOID(&amount), sizeof(amount));
			s.Close();
			return amount;
		}
		else { s.Close(); return 0; };
	}

	__declspec(dllexport) bool SocketStartThread(int evType)
	{
		AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
		AfxSocketInit();
		CSocket s;
		s.Create();

		if (s.Connect("127.0.0.1", 12345))
		{
			s.Send(LPCVOID(&evType), sizeof(evType));
			s.Close();
			return true;
		}
		else
		{
			s.Close();
			return false;
		}
	}
	int mod = 0;

	__declspec(dllexport) bool SocketSetMod(int modType)
	{
		AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
		AfxSocketInit();
		CSocket s;
		s.Create();
		if (s.Connect("127.0.0.1", 12345))
		{
			s.Send(LPCVOID(&modType), sizeof(modType));
			s.Close();
			return true;
		}
		else
		{
			s.Close();
			return false;
		}
	}


	__declspec(dllexport) bool SocketStopThread(int evType)
	{
		AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
		AfxSocketInit();
		CSocket s;
		s.Create();
		if (s.Connect("127.0.0.1", 12345))
		{
			s.Send(LPCVOID(&evType), sizeof(evType));
			s.Close();
			return true;
		}
		else
		{
			s.Close();
			return false;
		}
	}


	_declspec(dllexport) bool __stdcall SocketSend(int evType, int thId, const char* msg)
	{
		AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
		AfxSocketInit();
		CSocket s;
		s.Create();
		if (s.Connect("127.0.0.1", 12345))
		{
			int Size = strlen(msg) + 1;
			s.Send(LPCVOID(&evType), sizeof(evType));
			s.Send(LPCVOID(&thId), sizeof(thId));
			s.Send(LPCVOID(&Size), sizeof(Size));
			s.Send(LPCVOID(&msg), strlen(msg));
			s.Close();
			return true;
		}
		else
		{
			s.Close();
			return false;
		}
	}
}


BOOL CWindowsSocketApp::InitInstance()
{
	CWinApp::InitInstance();
	return TRUE;
}
