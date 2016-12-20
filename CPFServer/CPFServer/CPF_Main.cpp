////////////////////////////////////////////////
// iocpserver.cpp文件


// CIOCPServer类的测试程序

#include <stdio.h>

#include "CPF_ManagerAllServer.h"
int main()
{
    CPF_ManagerAllServer *pServer = new CPF_ManagerAllServer;

    // 开启服务
    pServer->InitModule();
    pServer->StartServer();

    getchar();
    // 关闭服务
    pServer->EndServer();
    pServer->UnitModule();
    delete pServer;
    printf(" 服务器关闭 \n ");
   
    return 0;
}


