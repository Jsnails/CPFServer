#include "CPF_EPOLLServer.h"
#include "../common/CPF_typedef.h"
#include "../CPFDataPacketParse/CPF_DataPacketParse.h"
#include "../common/CPF_IBuff.h"
#include "../common/CPF_Base.h"
#include <stdio.h>
#include <algorithm>

CPF_EPOLLServer::CPF_EPOLLServer()
{
    m_iConnectID = 1;
    m_mapEpollMutex = 
}

CPF_EPOLLServer::~CPF_EPOLLServer()
{

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
    bool bConnSuccss = false;

    /************************************************************************/
    /* 用户权限验证入口                                                       */
    /************************************************************************/
    if (m_pManagerServer)
    {
        CPF_IBuff IBuff;
        IBuff.m_pbuff = pbuff->pBuff;
        IBuff.m_nLen = pbuff->ilen;
        bConnSuccss = m_pManagerServer->OnConnectionEstablished(m_iConnectID, &IBuff);
    }

    /************************************************************************/
    /* 有权限则加入用户管理,没有权限则关闭用户连接                                */
    /************************************************************************/
    if (bConnSuccss)
    {
        OnConnectManagerAdd(iConnectID);
    }
    else
    {
        OnCloseAconnect(iConnectID);
    }

#ifdef CPF_TEST
    SendText(pContext, pBuffer->buff, pBuffer->nLen);//返回用户第一次发送数据
#endif
}

void CPF_EPOLLServer::OnConnectionClosing(CPF_UINT iConnectID, CPF_EPOLLBuffer *pbuff)
{

}

void CPF_EPOLLServer::OnConnectionError(CPF_UINT iConnectID, int nError)
{

}

void CPF_EPOLLServer::OnReadCompleted(CPF_UINT iConnectID, CPF_EPOLLBuffer *pbuff)
{

}

void CPF_EPOLLServer::OnWriteCompleted(CPF_UINT iConnectID, CPF_EPOLLBuffer *pbuff)
{

}

void CPF_EPOLLServer::SendToClient(CPF_UINT lConnectID, const char* pBuffer, const long lLen)
{

}

CPF_UINT CPF_EPOLLServer::OnConnectManagerAdd(CPF_UINT ulConnectID)
{

}

CPF_UINT CPF_EPOLLServer::OnConnectManagerSub(CPF_UINT ulConnectID)
{

}

CPF_UINT CPF_EPOLLServer::OnConnectManagerFind(CPF_UINT ulConnectID)
{

}

CPF_UINT CPF_EPOLLServer::OnConnectIDFind(CPF_UINT ulConnectID)
{

}


