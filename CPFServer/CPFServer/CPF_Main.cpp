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
    if (pServer->StartServer())
    {
        printf(" �����������ɹ�... \n");
    }
    else
    {
        printf(" ����������ʧ�ܣ�\n");
        return;
    }

    // �����¼�������ServerShutdown�����ܹ��ر��Լ�
    HANDLE hEvent = ::CreateEventA(NULL, FALSE, FALSE, "ShutdownEvent");
    ::WaitForSingleObject(hEvent, INFINITE);
    ::CloseHandle(hEvent);

    // �رշ���
    pServer->Shutdown();
    delete pServer;
    printf(" �������ر� \n ");
}
#else

#endif

