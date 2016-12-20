#include "CPF_EPOLLServer.h"
#include "../common/CPF_typedef.h"
#include "../common/CPF_IBuff.h"
#include "../common/CPF_Base.h"
#include <stdio.h>
#include <algorithm>

CPF_EPOLLServer::CPF_EPOLLServer()
{
    m_iConnectID = 1;
    m_mapEpollMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&m_mapEpollMutex,NULL);
}

CPF_EPOLLServer::~CPF_EPOLLServer()
{
   pthread_mutex_destroy(&m_mapEpollMutex);
}

void CPF_EPOLLServer::InitModule(CPF_Base *pBase)
{
    m_pManagerServer = pBase;
}

void CPF_EPOLLServer::UnitModule()
{

}

void CPF_EPOLLServer::OnConnectionEstablished(CPF_UINT iConnectID, CPF_EPOLLBuffer *pbuff)
{
    printf("oneconnect:%d\n",iConnectID);
    bool bConnSuccss = false;

    /************************************************************************/
    /* 用户权限验证入口                                                       */
    /************************************************************************/
    if (m_pManagerServer && pbuff != NULL)
    {
        CPF_IBuff IBuff;
        IBuff.m_pbuff = pbuff->pBuff;
        IBuff.m_nLen = pbuff->ilen;
        bConnSuccss = m_pManagerServer->OnConnectionEstablished(m_iConnectID, &IBuff);
    }

	if (pbuff == NULL)
	{
		bConnSuccss = true;
	}
    /************************************************************************/
    /* 有权限则加入用户管理,没有权限则关闭用户连接*/
    /************************************************************************/
    if (bConnSuccss)
    {
        OnConnectManagerAdd(iConnectID);
    }
    else
    {
        OnCloseAconnect(iConnectID);
    }

    //SendText(iConnectID, pbuff->pBuff, pbuff->ilen);//返回用户第一次发送数据
}

void CPF_EPOLLServer::OnConnectionClosing(CPF_UINT iConnectID, CPF_EPOLLBuffer *pbuff)
{
    printf("oneclose:%d\n",iConnectID);
    CPF_UINT uConnID = OnConnectManagerSub(iConnectID);
    if (m_pManagerServer && uConnID > 0)
    {
        m_pManagerServer->OnConnectionClosing(uConnID);
    }
}

void CPF_EPOLLServer::OnConnectionError(CPF_UINT iConnectID, int nError)
{
    CPF_UINT uConnID = OnConnectManagerSub(iConnectID);
    if (m_pManagerServer && uConnID > 0)
    {
        m_pManagerServer->OnConnectionError(uConnID,nError);
    }
}

void CPF_EPOLLServer::OnReadCompleted(CPF_UINT iConnectID, CPF_EPOLLBuffer *pbuff)
{
    CPF_UINT lConnectID = OnConnectIDFind(iConnectID);
    if (lConnectID == 0)
    {
        return;
    }
	printf("recv data:%d\n", iConnectID);
    if (m_pManagerServer)
    {
        CPF_IBuff Ibuff;
        Ibuff.m_pbuff = pbuff->pBuff;
        Ibuff.m_nLen = pbuff->ilen;
		printf("m_pManagerServer!=NULL");
       m_pManagerServer->OnReadCompleted(lConnectID, &Ibuff);
    }
}

void CPF_EPOLLServer::OnWriteCompleted(CPF_UINT iConnectID, CPF_EPOLLBuffer *pbuff)
{
    CPF_UINT lConnectID = OnConnectIDFind(iConnectID);
    if (m_pManagerServer && lConnectID > 0)
    {
        CPF_IBuff IBuff;
        IBuff.m_pbuff = pbuff->pBuff;
        IBuff.m_nLen = pbuff->ilen;
        m_pManagerServer->OnWriteCompleted(lConnectID, &IBuff);
    }
}

void CPF_EPOLLServer::SendToClient(CPF_UINT lConnectID, const char* pBuffer, const long lLen)
{
    CPF_UINT uConnID = OnConnectManagerFind(lConnectID);
    if(uConnID > 0)
    {
       SendText(uConnID,const_cast<char*>(pBuffer),lLen);
    }
}

CPF_UINT CPF_EPOLLServer::OnConnectManagerAdd(CPF_UINT ulConnectID)
{
   CPF_UINT uConnID = m_iConnectID;
   pthread_mutex_lock(&m_mapEpollMutex);
   m_mapEpollContext.insert(make_pair(m_iConnectID,ulConnectID));
   pthread_mutex_unlock(&m_mapEpollMutex);
   ++m_iConnectID;
   return uConnID;
}

CPF_UINT CPF_EPOLLServer::OnConnectManagerSub(CPF_UINT ulConnectID)
{
   CPF_UINT uConnID = 0;
    pthread_mutex_lock(&m_mapEpollMutex);
    map<CPF_UINT,CPF_UINT>::iterator ItemF = m_mapEpollContext.begin();
   for(;ItemF != m_mapEpollContext.end();ItemF++)
   {
        if(ItemF->second == ulConnectID)
        {
            uConnID = ItemF->first;
            m_mapEpollContext.erase(ItemF);
            break;
        }
   }
   pthread_mutex_unlock(&m_mapEpollMutex);
   return uConnID;
}

CPF_UINT CPF_EPOLLServer::OnConnectManagerFind(CPF_UINT ulConnectID)
{

    CPF_UINT uConnID = 0;
    pthread_mutex_lock(&m_mapEpollMutex);
    map<CPF_UINT,CPF_UINT>::iterator ItemF = m_mapEpollContext.begin();
   for(;ItemF != m_mapEpollContext.end();ItemF++)
   {
        if(ItemF->first == ulConnectID)
        {
            uConnID = ItemF->second;
            break;
        }
   }
   pthread_mutex_unlock(&m_mapEpollMutex);
   return uConnID;
}

CPF_UINT CPF_EPOLLServer::OnConnectIDFind(CPF_UINT ulConnectID)
{
    CPF_UINT uConnID = 0;
    pthread_mutex_lock(&m_mapEpollMutex);
    map<CPF_UINT,CPF_UINT>::iterator ItemF = m_mapEpollContext.begin();
    for(;ItemF != m_mapEpollContext.end();ItemF++)
    {
        if(ItemF->second == ulConnectID)
        {
            uConnID = ItemF->first;
            break;
        }
    }
   pthread_mutex_unlock(&m_mapEpollMutex);
   return uConnID;
}


