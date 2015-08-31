#include "CPF_IOCPServer.h"
#include "../common/CPF_typedef.h"
#include "../CPFDataPacketParse/CPF_DataPacketParse.h"
#include <stdio.h>
#include <algorithm>


CPF_IOCPServer::CPF_IOCPServer()
{
    m_pPacketParse = NULL;
    m_iConnectID = 1;

    InitializeCriticalSection(&m_mapContextSection);
}

CPF_IOCPServer::~CPF_IOCPServer()
{
    m_mapIOCPContext.clear();

    DeleteCriticalSection(&m_mapContextSection);
}

void CPF_IOCPServer::InitModule()
{
    if (m_pPacketParse == NULL)
    {
        m_pPacketParse = new CPF_DataPacketParse;
    }
}

void CPF_IOCPServer::UnitModule()
{
    if (m_pPacketParse)
    {
        delete m_pPacketParse;
    }
}

void CPF_IOCPServer::OnConnectionEstablished(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
    //printf("���յ�һ���µ�����(%d):%s \n", GetCurrentConnection(), ::inet_ntoa(pContext->addrRemote.sin_addr));
    bool bConnSuccss = false;

    /************************************************************************/
    /* �û�Ȩ����֤���                                                     */
    /************************************************************************/
    if (m_pPacketParse)
    {
        CPF_IBuff IBuff;
        IBuff.m_pbuff = pBuffer->buff;
        IBuff.m_nLen = pBuffer->nLen;
        bConnSuccss = m_pPacketParse->OneConnect(m_iConnectID,&IBuff);
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

#ifdef CPF_TEST
    SendText(pContext, pBuffer->buff, pBuffer->nLen);//�����û���һ�η�������
#endif
}

void CPF_IOCPServer::OnConnectionClosing(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
   //printf("һ�����ӹر�!��ʣ:(%d:%d)\n", GetCurrentConnection(), pContext->s);
    OnConnectManagerSub(pContext);
}

void CPF_IOCPServer::OnConnectionError(CIOCPContext *pContext, CIOCPBuffer *pBuffer, int nError)
{
     //printf("һ�����ӷ�������:%d \n ", nError);
    OnConnectManagerSub(pContext);
}

void CPF_IOCPServer::OnReadCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
    CPF_UINT lConnectID = OnConnectIDFind(pContext);
    if (lConnectID == 0)
    {
        return;
    }

    /************************************************************************/
    /* ���������                                                           */
    /************************************************************************/
    if (m_pPacketParse)
    {//����,������
        //m_pPacketParse->check
    }

    //��ȷ�İ�,����ģ��ַ��ӿ�

}

void CPF_IOCPServer::OnWriteCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
    //printf("���ݷ��ͳɹ�:%s\n",pBuffer);
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
    map<CPF_UINT, CIOCPContext*>::iterator itemM = m_mapIOCPContext.find(ulConnectID);
    if (itemM != m_mapIOCPContext.end())
    {
        LeaveCriticalSection(&m_mapContextSection);
        return NULL;
    }
    CIOCPContext* pContext = itemM->second;
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


