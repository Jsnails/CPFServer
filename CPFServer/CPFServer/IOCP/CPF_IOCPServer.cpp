#include "CPF_IOCPServer.h"
#include "../common/CPF_Base.h"
#include <stdio.h>
#include <algorithm>


CPF_IOCPServer::CPF_IOCPServer()
{
    m_iConnectID = 1;
    InitializeCriticalSection(&m_mapContextSection);
}

CPF_IOCPServer::~CPF_IOCPServer()
{
    m_mapIOCPContext.clear();
    DeleteCriticalSection(&m_mapContextSection);
}


void CPF_IOCPServer::InitModule(CPF_Base *pBase)
{
    m_pManagerServer = pBase;
}

void CPF_IOCPServer::UnitModule()
{

}

void CPF_IOCPServer::OnConnectionEstablished(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
    //printf("���յ�һ���µ�����(%d):%s \n", GetCurrentConnection(), ::inet_ntoa(pContext->addrRemote.sin_addr));
    bool bConnSuccss = false;

    /************************************************************************/
    /* �û�Ȩ����֤���                                                     */
    /************************************************************************/
    if (m_pManagerServer)
    {
        CPF_IBuff IBuff;
        IBuff.m_pbuff = pBuffer->buff;
        IBuff.m_nLen = pBuffer->nLen;
        bConnSuccss = m_pManagerServer->OnConnectionEstablished(m_iConnectID, &IBuff);
    }

    /************************************************************************/
    /* ��Ȩ��������û�����,û��Ȩ����ر��û�����                          */
    /************************************************************************/
    if (bConnSuccss)
    {
        OnConnectManagerAdd(pContext);
    }
    else
    {
        CloseAConnection(pContext);
    }

    SendText(pContext, pBuffer->buff, pBuffer->nLen);//�����û���һ�η�������
}

void CPF_IOCPServer::OnConnectionClosing(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
   //printf("һ�����ӹر�!��ʣ:(%d:%d)\n", GetCurrentConnection(), pContext->s);
    CPF_UINT uConnID = OnConnectIDFind(pContext);
    OnConnectManagerSub(pContext);
    if (m_pManagerServer)
    {
        m_pManagerServer->OnConnectionClosing(uConnID);
    }
}

void CPF_IOCPServer::OnConnectionError(CIOCPContext *pContext, CIOCPBuffer *pBuffer, int nError)
{
     //printf("һ�����ӷ�������:%d \n ", nError);
    CPF_UINT uConnID = OnConnectIDFind(pContext);
    OnConnectManagerSub(pContext);
    if (m_pManagerServer)
    {
        m_pManagerServer->OnConnectionError(uConnID,nError);
    }
}

void CPF_IOCPServer::OnReadCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
    CPF_UINT lConnectID = OnConnectIDFind(pContext);
    if (lConnectID == 0)
    {
        return;
    }

    if (m_pManagerServer)
    {//����,������
        CPF_IBuff Ibuff;
        Ibuff.m_pbuff = pBuffer->buff;
        Ibuff.m_nLen = pBuffer->nLen;
  
       m_pManagerServer->OnReadCompleted(lConnectID, &Ibuff);
    }
}

void CPF_IOCPServer::OnWriteCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
    //printf("���ݷ��ͳɹ�:%s\n",pBuffer);
    if (m_pManagerServer)
    {
        CPF_UINT lConnectID = OnConnectIDFind(pContext);
        CPF_IBuff IBuff;
        IBuff.m_pbuff = pBuffer->buff;
        IBuff.m_nLen = pBuffer->nLen;
        m_pManagerServer->OnWriteCompleted(lConnectID, &IBuff);
    }
}

void CPF_IOCPServer::SendToClient(CPF_UINT lConnectID, const char* pBuffer, const long lLen)
{
    CIOCPContext *pContext = OnConnectManagerFind(lConnectID);
    if (pContext != NULL)
    {
        SendText(pContext,const_cast<char*>(pBuffer), lLen);
    }
}

CIOCPContext * CPF_IOCPServer::OnConnectManagerAdd(CIOCPContext *pContext)
{
    if (pContext == NULL)
    {
        return pContext;
    }
    EnterCriticalSection(&m_mapContextSection);
    auto pInsert = m_mapIOCPContext.insert(make_pair(m_iConnectID, pContext));
    LeaveCriticalSection(&m_mapContextSection);
    if (pInsert.second)
    {
        m_iConnectID++;//�ȴ���һ�����ֵ���
    }
    else
    {
        pContext = NULL;
    }
    return pContext;
}

CIOCPContext * CPF_IOCPServer::OnConnectManagerSub(CIOCPContext *pContext)
{
    if (pContext == NULL)
    {
        return pContext;
    }

    EnterCriticalSection(&m_mapContextSection);
    for (map<CPF_UINT, CIOCPContext*>::iterator itemM = m_mapIOCPContext.begin(); itemM != m_mapIOCPContext.end(); itemM++)
    {
        CIOCPContext* pCon = itemM->second;
        if (pCon->s == pContext->s)
        {
            m_mapIOCPContext.erase(itemM);
            break;
        }
    }
    LeaveCriticalSection(&m_mapContextSection);

    return pContext;
}

CIOCPContext * CPF_IOCPServer::OnConnectManagerFind(CPF_UINT ulConnectID)
{
    EnterCriticalSection(&m_mapContextSection);
    map<CPF_UINT, CIOCPContext*>::iterator itemM = m_mapIOCPContext.begin();
    CIOCPContext* pContext = NULL;
    for (; itemM != m_mapIOCPContext.end();itemM++)
    {
        if (itemM->first == ulConnectID)
        {
            pContext = itemM->second;
            break;
        }
    }

    LeaveCriticalSection(&m_mapContextSection);

    return pContext;
}

CPF_UINT CPF_IOCPServer::OnConnectIDFind(CIOCPContext *pContext)
{
    CPF_UINT ulConnectID = 0;

    EnterCriticalSection(&m_mapContextSection);
    for (map<CPF_UINT, CIOCPContext*>::iterator itemM = m_mapIOCPContext.begin(); itemM != m_mapIOCPContext.end();itemM++)
    {
        CIOCPContext *pConn = itemM->second;
        if (pConn->s == pContext->s)
        {
            ulConnectID = itemM->first;
            break;
        }
    }
    LeaveCriticalSection(&m_mapContextSection);

    return ulConnectID;
}


