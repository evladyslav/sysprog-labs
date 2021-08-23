#include <iostream>
#include <thread>
#include <vector>
#include <Windows.h>
#include <conio.h>
#include "tools.h"


DWORD WINAPI YeahThread(LPVOID lpParameter)
{
    int id = int(lpParameter);
    auto str = "EventStop" + strtr(id);
    HANDLE hMutex = CreateMutex(NULL, FALSE, "YeahMutex");
    HANDLE hEventPrint = CreateEvent(NULL, TRUE, FALSE, "EventPrint");
    HANDLE hEventStop = CreateEvent(NULL, TRUE, FALSE, str.c_str());

    WaitForSingleObject(hMutex, INFINITE);
    std::cout << "\nThread started  " << strtr(id) << std::endl;
    ReleaseMutex(hMutex);

    HANDLE hEvents[] = { hEventPrint, hEventStop };
    while (true) {
        switch (WaitForMultipleObjects(2, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0)
        {
        case 0:
        {
            WaitForSingleObject(hMutex, INFINITE);
            std::cout << "\nHello from  " << strtr(id) << std::endl;
            ReleaseMutex(hMutex);
            break;
        }
        case 1:
        {
            ResetEvent(hEventStop);
            WaitForSingleObject(hMutex, INFINITE);
            std::cout << "\nThread ended  " << strtr(id) << std::endl;
            ReleaseMutex(hMutex);
            return 0;
        }
        }
    }
    return 0;
}


void start() {
    int i = 0;
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    auto hMutex = CreateMutex(NULL, FALSE, "YeahMutex");
    auto hEventStart = CreateEvent(NULL, TRUE, FALSE, "EventStart");
    auto hEventStop = CreateEvent(NULL, TRUE, FALSE, "EventStop");
    auto evSubmit = CreateEvent(NULL, TRUE, FALSE, "ThreadSubmit");

    HANDLE hEvents[] = { hEventStart, hEventStop };


    while (true) {
        switch (WaitForMultipleObjects(2, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0)
        {

        case 0:
        {
            ResetEvent(hEventStart);
            //HANDLE hThread = AfxBeginThread();
            std::thread t(YeahThread, (LPVOID)i++);
            t.detach();
            //HANDLE hThread = CreateThread(NULL, 0, YeahThread, (LPVOID)i++, 0, NULL);
            SetEvent(evSubmit);
            break;
        }
        case 1:
        {
            if (i == 0)
            {
                SetEvent(evSubmit);
                return;
            }
            else
            {
                i--;
                auto str = "EventStop" + strtr(i);
                HANDLE hlEventStop = CreateEvent(NULL, TRUE, FALSE, str.c_str());
                SetEvent(hlEventStop);
                ResetEvent(hlEventStop);
                SetEvent(evSubmit);
            }
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
    start();
    return 0;
}
