#ifdef WIN32
#include "./IOCP/CPF_IOCPServer.h"
#else
#include "./EPOLL/CPF_EPOLLServer.h"
#endif
#include "CPF_ManagerAllServer.h"
#include "./CPFDataPacketParse/CPF_DataPacketParse.h"
#include "./DataPool/CPF_IManagerBUffer.h"



CPF_ManagerAllServer::CPF_ManagerAllServer()
{
    m_pPacketParse = NULL;

#ifdef WIN32
    m_pIOCPServer = NULL;
#else
    m_pEpollServer = NULL;
#endif
}


CPF_ManagerAllServer::~CPF_ManagerAllServer()
{

}

bool CPF_ManagerAllServer::OnConnectionEstablished(CPF_UINT ulConnectID, CPF_IBuff *pUserData)
{
    if (m_pPacketParse)
    {
        return m_pPacketParse->OneConnect(ulConnectID, pUserData);
    }
    return false;
}

void CPF_ManagerAllServer::OnConnectionClosing(CPF_UINT ulConnectID)
{
    //通知使用模块，用户模块连接关闭了。
}

void CPF_ManagerAllServer::OnConnectionError(CPF_UINT ulConnectID, int nError)
{
    //通知用户模块一个连接已经出错了
}

void CPF_ManagerAllServer::OnReadCompleted(CPF_UINT ulConnectID, CPF_IBuff *pUserData)
{
    /************************************************************************/
    /* 解析获得数据包                                                       */
    /************************************************************************/
    bool bIsComplete = false;
    if (m_pPacketParse)
    {
        if (m_pPacketParse->GetPacketModule() == CPF_GETSTREAM)
        {
            //数据流解析
            bIsComplete = m_pPacketParse->CheckRecvPacketParse(CPF_IManagerBUffer::GetInstance(), ulConnectID, pUserData);
        }
        else
        {
            //自定义包解析
        }
    }
    //正确的包,进入模块分发接口
    if (bIsComplete)
    {
        switch (m_pPacketParse->GetPacketModule())
        {
            case CPF_GETSTREAM:
            {
                                  //数据流通知用户
                                  if (m_pIOCPServer)
                                  {
                                      m_pIOCPServer->SendToClient(ulConnectID, pUserData->m_pbuff, pUserData->m_nLen);
                                  }
            }
            break;
            case CPF_GETHEAD:
            {
                                //自定义数据包通知用户
            }
            break;
            default:
            break;
        }
    }
}

void CPF_ManagerAllServer::OnWriteCompleted(CPF_UINT ulConnectID, CPF_IBuff *pUserData)
{
    //通知用户写操作完成
}

void CPF_ManagerAllServer::InitModule()
{
    if (m_pPacketParse == NULL)
    {
        m_pPacketParse = new CPF_DataPacketParse;
    }
#ifdef WIN32
    if (m_pIOCPServer == NULL)
    {
        m_pIOCPServer = new CPF_IOCPServer;
        m_pIOCPServer->InitModule((CPF_Base *)this);
    }
#else
    if (m_pEpollServer == NULL)
    {
        m_pEpollServer = new CPF_EPOLLServer;
    }
#endif
    
}

void CPF_ManagerAllServer::UnitModule()
{
    if (m_pPacketParse)
    {
        delete m_pPacketParse;
    }
#ifdef WIN32
    if (m_pIOCPServer)
    {
        m_pIOCPServer->UnitModule();
        delete m_pIOCPServer;
        m_pIOCPServer = NULL;
    }
#else
    if (m_pEpollServer)
    {
        m_pEpollServer->UnitModule();
        delete m_pEpollServer;
        m_pEpollServer = NULL;
    }
#endif
}

bool CPF_ManagerAllServer::StartServer()
{
#ifdef WIN32
    if (m_pIOCPServer)
    {
      return  m_pIOCPServer->StartServer();
    }
#else
    if (m_pEpollServer)
    {
       return  m_pEpollServer->StartServer();
    }
#endif
    return true;
}

bool CPF_ManagerAllServer::EndServer()
{
#ifdef WIN32
    if (m_pIOCPServer)
    {
         m_pIOCPServer->Shutdown();
    }
#else
    if (m_pEpollServer)
    {
       m_pEpollServer->EndServer();
    }
#endif
    return true;
}
