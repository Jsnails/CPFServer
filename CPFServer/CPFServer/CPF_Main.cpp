////////////////////////////////////////////////
// iocpserver.cpp�ļ�


// CIOCPServer��Ĳ��Գ���

#include <stdio.h>

#include "CPF_ManagerAllServer.h"
int main()
{
    CPF_ManagerAllServer *pServer = new CPF_ManagerAllServer;

    // ��������
    pServer->InitModule();
    pServer->StartServer();

    getchar();
    // �رշ���
    pServer->EndServer();
    pServer->UnitModule();
    delete pServer;
    printf(" �������ر� \n ");
   
    return 0;
}


