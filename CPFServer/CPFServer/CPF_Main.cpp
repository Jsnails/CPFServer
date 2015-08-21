////////////////////////////////////////////////
// iocpserver.cpp文件


// CIOCPServer类的测试程序

#include <stdio.h>
#include "./iocp/CPF_IOCPServer.h"


#ifdef WIN32
void main()
{
    CPF_IOCPServer *pServer = new CPF_IOCPServer;

    // 开启服务
    if (pServer->StartServer())
    {
        printf(" 服务器开启成功... \n");
    }
    else
    {
        printf(" 服务器开启失败！\n");
        return;
    }

    // 创建事件对象，让ServerShutdown程序能够关闭自己
    HANDLE hEvent = ::CreateEventA(NULL, FALSE, FALSE, "ShutdownEvent");
    ::WaitForSingleObject(hEvent, INFINITE);
    ::CloseHandle(hEvent);

    // 关闭服务
    pServer->Shutdown();
    delete pServer;
    printf(" 服务器关闭 \n ");
}
#else

#endif

