#include "pch.h"
#include "framework.h"
#include "lab4cpp.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PIPE_NAME (LPCSTR)"\\\\.\\pipe\\YeahPipe"

CWinApp theApp;

using namespace std;


UINT __cdecl YeahThread(LPVOID lpParameter)
{
    int id = int(lpParameter);
    auto StopName = "EventStop" + InttoStr(id);

    HANDLE hEventStop = CreateEvent(NULL, TRUE, FALSE, StopName.c_str());
    HANDLE hMutex = CreateMutex(NULL, FALSE, "YeahMutex");
   
    HANDLE hEvents[] = { hEventStop };

    WaitForSingleObject(hMutex, INFINITE);
    std::cout << "\nThread started  " << InttoStr(id) << std::endl;
    ReleaseMutex(hMutex);
    while (true)
    {
        switch (WaitForMultipleObjects(1, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0)
        {
           
            case 0:
            {
                ResetEvent(hEventStop);
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
    
    while (true)
    {
        int evType = NULL;
        HANDLE hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 1024, 1024, 0, NULL);
        HANDLE hMutex = CreateMutex(NULL, FALSE, "YeahMutex");
        DWORD dwRead, dwWrite;
        ConnectNamedPipe(hPipe, NULL);
        ReadFile(hPipe, LPVOID(&evType), sizeof(evType), &dwRead, NULL);
        switch (evType)
        {
        case 0:
        {
           
            thread t(YeahThread, (LPVOID)thread_index++);
            t.detach();
            FlushFileBuffers(hPipe);
            DisconnectNamedPipe(hPipe);
            break;
        }

        case 1:
        {
            
            if (!(thread_index == 1))
            {
                auto StopName = "EventStop" + InttoStr(thread_index-- - 1);
                auto evStop = CreateEvent(NULL, TRUE, FALSE, StopName.c_str());
                SetEvent(evStop);
                CloseHandle(evStop);
                break;
            }
            FlushFileBuffers(hPipe);
            DisconnectNamedPipe(hPipe);
            break;
        }

        case 2:
        {
            const int MAXLEN = 1024;
            int thId = 0;
            DWORD msgRead;
            char buff[MAXLEN + 1];

            ReadFile(hPipe, LPVOID(&thId), sizeof(thId), &msgRead, NULL);
            ReadFile(hPipe, buff, MAXLEN, &msgRead, NULL);
            FlushFileBuffers(hPipe);
            buff[min(MAXLEN, msgRead)] = 0;
            string msg(buff);
                        
            if (!thId == 0)
            {
                WriteToFile(thId, msg.c_str());
                DisconnectNamedPipe(hPipe);
                CloseHandle(hPipe);
            }
            else
            {
                WaitForSingleObject(hMutex, INFINITE);
                std::cout << "\nReceived Message  " << msg << " from:" << thId << std::endl;
                ReleaseMutex(hMutex);  
                DisconnectNamedPipe(hPipe);
                CloseHandle(hPipe);
            }
            break;
        }
        case 3:
        {
            WriteFile(hPipe, LPCVOID(&thread_index), sizeof(thread_index), &dwWrite, NULL);
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