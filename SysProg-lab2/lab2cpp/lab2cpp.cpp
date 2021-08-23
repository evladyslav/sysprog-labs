#include "pch.h"
#include "framework.h"
#include "lab2cpp.h"
#include <iostream>
#include <thread>
#include <vector>
#include <Windows.h>
#include <conio.h>
#include <fstream>
#include "..\Message\Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;

struct Message 
{ 
int index; 
char data[]; 
};

HMODULE hLib = LoadLibrary("Message.dll");
auto readMap = (Message*(*)())GetProcAddress(hLib, "readMap");

std::string toStr(int i)
{
    std::string s;
    std::stringstream out;
    out << i;
    s = out.str();
    return s;
}


void WriteFile(int index, std::string name) 
{
    Message* pMessage = readMap();
    CString filename;
    filename.Format("..\\%d.txt", index);
    std::ofstream f;
    f.open(filename, std::ios::app);
    std::string msg = pMessage->data;
    f << msg.c_str() << std::endl;
    f.close();
    std::cout << "\nThread " + toStr(index) + " created file " << std::endl;
}


UINT __cdecl YeahThread(LPVOID lpParameter)
{
    int id = int(lpParameter);
    auto StopName = "EventStop" + toStr(id);
    auto MessageName = "NewMessage" + toStr(id);
    HANDLE hMutex = CreateMutex(NULL, FALSE, "YeahMutex");
    HANDLE hEventPrint = CreateEvent(NULL, TRUE, FALSE, "EventPrint");
    HANDLE hEventStop = CreateEvent(NULL, TRUE, FALSE, StopName.c_str());
    HANDLE hFile = CreateEvent(NULL, TRUE, FALSE, MessageName.c_str());
    WaitForSingleObject(hMutex, INFINITE);
    std::cout << "\nThread started  " << toStr(id) << std::endl;
    ReleaseMutex(hMutex);
    HANDLE hEvents[] = { hEventPrint, hEventStop,  hFile };
    while (true) {
        switch (WaitForMultipleObjects(3, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0)
        {
            case 0:
            {
                SetEvent(hEventPrint);
                WaitForSingleObject(hMutex, INFINITE);
                std::cout << "\nHello from  " << toStr(id) << std::endl;
                ReleaseMutex(hMutex);
                break;
            }
            case 1:
            {

                ResetEvent(hEventStop);
                WaitForSingleObject(hMutex, INFINITE);
                std::cout << "\nThread ended  " << toStr(id) << std::endl;
                ReleaseMutex(hMutex);
                return 0;
            }
            case 2:
            {
                WaitForSingleObject(hMutex, INFINITE);
                WriteFile(id, "Thread");
                ResetEvent(hFile);
                ReleaseMutex(hMutex);
                break;
            }
        }
    }
    return 0;
}


void start() {
    int thread_index = 1;
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    auto hMutex = CreateMutex(NULL, FALSE, "YeahMutex");
    auto hEventStart = CreateEvent(NULL, TRUE, FALSE, "EventStart");
    auto hEventStop = CreateEvent(NULL, TRUE, FALSE, "EventStop");
    auto evSubmit = CreateEvent(NULL, TRUE, FALSE, "ThreadSubmit");
    auto evMain = CreateEvent(NULL, TRUE, FALSE, "evMain");
    HANDLE hEvents[] = { hEventStart, hEventStop, evMain};
    while (true) {
        switch (WaitForMultipleObjects(3, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0)
        {
        case 0:
        {
            ResetEvent(hEventStart);
            HANDLE hThread = AfxBeginThread(YeahThread, (LPVOID)thread_index++);
            SetEvent(evSubmit);
            break;
        }
        case 1:
        {
            if (thread_index == 1)
            {
                SetEvent(evSubmit);
                return;
            }
            else
            {
                thread_index--;
                auto StopName = "EventStop" + toStr(thread_index);
                HANDLE hlEventStop = CreateEvent(NULL, TRUE, FALSE, StopName.c_str());
                SetEvent(hlEventStop);
               // ResetEvent(hlEventStop);
                SetEvent(evSubmit);
            }
            break;
        }
        case 2:
        {
            WaitForSingleObject(hMutex, INFINITE);
            Message* pMessage = readMap();
            std::string d(pMessage->data);
            std::cout << d << std::endl;
            ResetEvent(evMain);
            ReleaseMutex(hMutex);
            break;
        }
        }
    }

    while (true) {
        SetEvent(hEventStart);
        ResetEvent(hEventStart);
    }
    CloseHandle(hMutex);
    CloseHandle(hEventStart);
    CloseHandle(hEventStop);
    CloseHandle(evSubmit);
}


int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            start();
            // TODO: вставьте сюда код для приложения.
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}