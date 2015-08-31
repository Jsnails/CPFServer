////////////////////////////////////////////////
// iocpserver.cpp�ļ�


// CIOCPServer��Ĳ��Գ���

#include <stdio.h>

#ifdef WIN32
#include "./IOCP/CPF_IOCPServer.h"
void main()
{
    CPF_IOCPServer *pServer = new CPF_IOCPServer;

    // ��������
    pServer->InitModule();
    pServer->StartServer();

    getchar();
    // �رշ���
    pServer->Shutdown();
    pServer->UnitModule();
    delete pServer;
    printf(" �������ر� \n ");
}
#else
#include "./EPOLL/CPF_EPOLL.h"
int main()
{
    CPF_EPOLL ep;
    ep.StartServer();
    getchar();
    return 0;
}
#endif

