#include "pch.h"
#include "framework.h"
#include "lab4cpp.h"
#include <afxsock.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PIPE_NAME (LPCSTR)"\\\\.\\pipe\\YeahPipe"

CWinApp theApp;

using namespace std;


DWORD WINAPI YeahThread(LPVOID lpParameter)//, SOCKET hSock)
{
    //CSocket s;
    //s.Attach(hSock);
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

void npStart()
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

void socketStart() 
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
                thread t(YeahThread, (LPVOID)thread_index++);//, s.Detach());
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
                unsigned char buff[MAXLEN];
                s.Receive(LPVOID(&thId), sizeof(thId));
                s.Receive(LPVOID(&strSize), sizeof(strSize));
                int trash;
                s.Receive(LPVOID(&trash), sizeof(trash));
                vector <char> v(strSize);
                s.Receive(LPVOID(&v[0]), strSize);
                string msg(&v[0]);
   
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
       
        s.Close();
    }
    Server.Close();
    
    
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
            while (true) 
            {
                AfxSocketInit();
                CSocket Server;
                Server.Create(12345);
                Server.Listen();
                int chMode = NULL;

                CSocket s;
                Server.Accept(s);
                int evType = NULL;
                s.Receive(LPVOID(&chMode), sizeof(chMode));
                if (chMode == 1)
                {
                    Server.Close();
                    s.Close();
                    socketStart();
                    return 0;
                }
                else if (chMode == 2)
                {
                    Server.Close();
                    s.Close();
                    npStart();
                    return 0;
                }
                
            }

        }
    }
    else
    {
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}