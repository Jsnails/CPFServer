#ifndef __CPF_IOCPServer_H_
#define __CPF_IOCPServer_H_

#include "CPF_IOCP.H"

class CPF_IOCPServer :public CIOCPServer
{ 
public:
    CPF_IOCPServer();
    ~CPF_IOCPServer();

    /************************************************************************/
    /*// �¼�֪ͨ����                                                       */
    /************************************************************************/
    // ������һ���µ�����
    void OnConnectionEstablished(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
    // һ�����ӹر�
    void OnConnectionClosing(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
    // ��һ�������Ϸ����˴���
    void OnConnectionError(CIOCPContext *pContext, CIOCPBuffer *pBuffer, int nError);
    // һ�������ϵĶ��������
    void OnReadCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
    // һ�������ϵ�д�������
    void OnWriteCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer);

    /************************************************************************/
    /*�����¼�                                                              */
    /************************************************************************/
    virtual void SendToClient(unsigned long lConnectID, const char* pBuffer, const long lLen);
};

#endif __CPF_IOCPServer_H_