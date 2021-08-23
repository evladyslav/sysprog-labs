#include "pch.h"
#include "framework.h"
#include "lab4cpp.h"
#include <afxsock.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CWinApp theApp;

using namespace std;


DWORD WINAPI YeahThread(LPVOID lpParameter, SOCKET hSock)
{
    CSocket s;
    s.Attach(hSock);
    int id = int(lpParameter);
    auto MessageName = "EventStop" + InttoStr(id);
    HANDLE evStop = CreateEvent(NULL, TRUE, FALSE, MessageName.c_str());
    HANDLE hMutex = CreateMutex(NULL, FALSE, "YeahMutex");
    HANDLE hEvents[] = { evStop };
    WaitForSingleObject(hMutex, INFINITE);
    std::cout << "\nThread started  " << InttoStr(id) << std::endl;
    ReleaseMutex(hMutex);
    while (true)
    {      
        switch (WaitForMultipleObjects(1, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0)
        {
            case 0:
            {          
                ResetEvent(evStop);
                WaitForSingleObject(hMutex, INFINITE);
                std::cout << "\nThread stopped  " << InttoStr(id) << std::endl;
                ReleaseMutex(hMutex);
                return 0;
            }
            
        }
    }
    return 0;
}


void start() 
{
    int thread_index = 1;
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    HANDLE hMutex = CreateMutex(NULL, FALSE, "YeahMutex");
    AfxSocketInit();
    CSocket Server;
    Server.Create(12345);
    Server.Listen();

    while (true)
    {
        CSocket s; 
        Server.Accept(s);
        int evType = NULL;
        s.Receive(LPVOID(&evType), sizeof(evType));
        switch (evType)
        {
            case 0:
            {
                thread t(YeahThread, (LPVOID)thread_index++, s.Detach());
                t.detach();
                break;
            }

            case 1:
            {
                if (!(thread_index == 1))
                {
                    auto MessageName = "EventStop" + InttoStr(thread_index-- - 1);
                    auto evStop = CreateEvent(NULL, TRUE, FALSE, MessageName.c_str());
                    SetEvent(evStop);
                    CloseHandle(evStop);
                    break;
                }
                break;

            }

            case 2:
            {
                const int MAXLEN = 1024;
                int thId, strSize = 0;
                char buff[MAXLEN + 1];
                s.Receive(LPVOID(&thId), sizeof(thId));
                s.Receive(LPVOID(&strSize), sizeof(strSize));
                s.Receive(LPVOID(&buff), MAXLEN);
                buff[MAXLEN] = 0;
                string msg(buff);

                if (!thId == 0)
                {
                    WriteToFile(thId, msg.c_str());
                }
                else
                {
                    WaitForSingleObject(hMutex, INFINITE);
                    std::cout << "\nReceived Message  " << msg << " from:" << thId << std::endl;
                    ReleaseMutex(hMutex);
                }
                break;
            }
            case 3: 
            {
                s.Send(LPCVOID(&thread_index), sizeof(thread_index));
            }
        }
       
        
    }
    
}


int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            start();
        }
    }
    else
    {
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}