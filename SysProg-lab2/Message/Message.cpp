#include "pch.h"
#include "framework.h"
#include "Message.h"
#include <cstdint>
#include <filesystem>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CMessageApp, CWinApp)
END_MESSAGE_MAP()

CMessageApp::CMessageApp()
{
	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
}

CMessageApp theApp;

using namespace std;

struct Message 
{
	int i;
	char s[];
};

LPVOID fileMap;
LPVOID lpFileMap;

BOOL CMessageApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

HANDLE mMutex = CreateMutex(NULL, FALSE, "YeahMutex");

extern "C"
{
	__declspec(dllexport) bool writeMap(char* Str, int Id=0)
	{
		int length = strlen(Str);
		fileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, length, "Map");
		LPVOID lpFileMap = MapViewOfFile(fileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		Message* pMsg = ((Message*)lpFileMap);
		pMsg -> i = Id;
		strcpy(pMsg -> s, Str);
		pMsg -> s [length] = 0;
		UnmapViewOfFile(lpFileMap);
		return true;
	}

	__declspec(dllexport) Message* readMap()
	{
		fileMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, "Map");
		lpFileMap = MapViewOfFile(fileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		return ((Message*)lpFileMap);
	}

	__declspec(dllexport) void closeMap()
	{
		UnmapViewOfFile(lpFileMap);
		CloseHandle(fileMap);
	}
}