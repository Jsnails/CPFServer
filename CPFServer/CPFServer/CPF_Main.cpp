////////////////////////////////////////////////
// iocpserver.cpp�ļ�


// CIOCPServer��Ĳ��Գ���

#include <stdio.h>
#include "./iocp/CPF_IOCPServer.h"


#ifdef WIN32
void main()
{
    CPF_IOCPServer *pServer = new CPF_IOCPServer;

    // ��������
    pServer->InitModule();
    if (pServer->StartServer())
    {
        printf(" �����������ɹ�... \n");
    }
    else
    {
        printf(" ����������ʧ�ܣ�\n");
        return;
    }

    getchar();
    // �رշ���
    pServer->Shutdown();
    pServer->UnitModule();
    delete pServer;
    printf(" �������ر� \n ");
}
#else

#endif

