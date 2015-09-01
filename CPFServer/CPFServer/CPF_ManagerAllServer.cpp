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
    //֪ͨʹ��ģ�飬�û�ģ�����ӹر��ˡ�
}

void CPF_ManagerAllServer::OnConnectionError(CPF_UINT ulConnectID, int nError)
{
    //֪ͨ�û�ģ��һ�������Ѿ�������
}

void CPF_ManagerAllServer::OnReadCompleted(CPF_UINT ulConnectID, CPF_IBuff *pUserData)
{
    /************************************************************************/
    /* ����������ݰ�                                                       */
    /************************************************************************/
    bool bIsComplete = false;
    if (m_pPacketParse)
    {
        if (m_pPacketParse->GetPacketModule() == CPF_GETSTREAM)
        {
            //����������
            bIsComplete = m_pPacketParse->CheckRecvPacketParse(CPF_IManagerBUffer::GetInstance(), ulConnectID, pUserData);
        }
        else
        {
            //�Զ��������
        }
    }
    //��ȷ�İ�,����ģ��ַ��ӿ�
    if (bIsComplete)
    {
        switch (m_pPacketParse->GetPacketModule())
        {
            case CPF_GETSTREAM:
            {
                                  //������֪ͨ�û�
                                  if (m_pIOCPServer)
                                  {
                                      m_pIOCPServer->SendToClient(ulConnectID, pUserData->m_pbuff, pUserData->m_nLen);
                                  }
            }
            break;
            case CPF_GETHEAD:
            {
                                //�Զ������ݰ�֪ͨ�û�
            }
            break;
            default:
            break;
        }
    }
}

void CPF_ManagerAllServer::OnWriteCompleted(CPF_UINT ulConnectID, CPF_IBuff *pUserData)
{
    //֪ͨ�û�д�������
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
