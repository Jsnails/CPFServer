#include "CPF_IOCPServer.h"
#include <stdio.h>

CPF_IOCPServer::CPF_IOCPServer()
{

}

CPF_IOCPServer::~CPF_IOCPServer()
{

}

void CPF_IOCPServer::OnConnectionEstablished(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
    //printf("���յ�һ���µ�����(%d):%s \n", GetCurrentConnection(), ::inet_ntoa(pContext->addrRemote.sin_addr));
    SendText(pContext, pBuffer->buff, pBuffer->nLen);//test
}

void CPF_IOCPServer::OnConnectionClosing(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
   //printf("һ�����ӹر�!��ʣ:(%d:%d)\n", GetCurrentConnection(), pContext->s);
}

void CPF_IOCPServer::OnConnectionError(CIOCPContext *pContext, CIOCPBuffer *pBuffer, int nError)
{
     //printf("һ�����ӷ�������:%d \n ", nError);
}

void CPF_IOCPServer::OnReadCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
     SendText(pContext, pBuffer->buff, pBuffer->nLen);//test
}

void CPF_IOCPServer::OnWriteCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
    //printf("���ݷ��ͳɹ�:%s\n",pBuffer);
}

void CPF_IOCPServer::SendToClient(unsigned long lConnectID, const char* pBuffer, const long lLen)
{

}
