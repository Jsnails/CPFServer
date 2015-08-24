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
    pServer->InitModule();
    if (pServer->StartServer())
    {
        printf(" 服务器开启成功... \n");
    }
    else
    {
        printf(" 服务器开启失败！\n");
        return;
    }

    getchar();
    // 关闭服务
    pServer->Shutdown();
    pServer->UnitModule();
    delete pServer;
    printf(" 服务器关闭 \n ");
}
#else

#endif

