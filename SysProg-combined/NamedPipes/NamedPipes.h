// NamedPipes.h: основной файл заголовка для библиотеки DLL NamedPipes
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CNamedPipesApp
// Реализацию этого класса см. в файле NamedPipes.cpp
//

class CNamedPipesApp : public CWinApp
{
public:
	CNamedPipesApp();

// Переопределение
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
