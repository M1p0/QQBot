#include "Http_Server.h"
#include <CFileIO.h>
#include <MSocket.h>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <iostream>
#include <MyEvent.h>
#include <random>
#include "Functions.h"
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )  //隐藏dos窗口
using namespace std;




MSocket sock;
CFileIO File;
unordered_map<string, ID_List> message;
std::mutex mtx_message;



int Program_Mutex()
{
    HANDLE hMutex = NULL;
    LPCTSTR lpszName = (LPCTSTR)"AppMutex";
    int nRet = 0;

    hMutex = ::CreateMutex(NULL, FALSE, lpszName);
    DWORD dwRet = ::GetLastError();

    switch (dwRet)
    {
    case 0:
    {
        break;
    }
    case ERROR_ALREADY_EXISTS:
    {
        cout << "program is already running" << endl;
        nRet = 1;
        break;
    }
    default:
    {
        cout << "create mutex failed" << endl;
        nRet = -1;
        break;
    }
    }
    return nRet;
}




int main()
{
    if (Program_Mutex() != 0)   //确保只有一个实例
        return 0;
    thread TRepeatBan(RepeatBan);
    TRepeatBan.detach();
    startHttpServer("0.0.0.0", 9001, MyHttpServerHandler, NULL);
    return 0;


}


