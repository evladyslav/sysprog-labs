// WindowsSocket.h: основной файл заголовка для библиотеки DLL WindowsSocket
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CWindowsSocketApp
// Реализацию этого класса см. в файле WindowsSocket.cpp
//

class CWindowsSocketApp : public CWinApp
{
public:
	CWindowsSocketApp();

// Переопределение
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
