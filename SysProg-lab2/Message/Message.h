// Message.h: основной файл заголовка для библиотеки DLL Message
//

#pragma once



#include "resource.h"		// основные символы
#include <fstream>

// CMessageApp
// Реализацию этого класса см. в файле Message.cpp
//




class CMessageApp : public CWinApp
{
public:
	CMessageApp();

		
// Переопределение
public:
	virtual BOOL InitInstance();
	


	DECLARE_MESSAGE_MAP()
};

