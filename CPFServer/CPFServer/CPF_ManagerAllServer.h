#ifndef __CPF_ManagerAllServer_H_
#define __CPF_ManagerAllServer_H_

#include "common/CPF_Base.h"

class CPF_DataPacketParse;
#ifdef WIN32
class CPF_IOCPServer;
#else
class CPF_EPOLLServer
#endif
class CPF_ManagerAllServer : public CPF_Base
{
public:
    CPF_ManagerAllServer();
    ~CPF_ManagerAllServer();

    void InitModule();
    void UnitModule();

    bool StartServer();
    bool EndServer();
    /************************************************************************/
    /* �����¼� */
    /************************************************************************/
    // ������һ���µ�����
    virtual bool OnConnectionEstablished(CPF_UINT  ulConnectID, CPF_IBuff *pUserData);
    // һ�����ӹر�
    virtual void OnConnectionClosing(CPF_UINT  ulConnectID);
    // ��һ�������Ϸ����˴���
    virtual void OnConnectionError(CPF_UINT  ulConnectID, int nError);
    // һ�������ϵĶ��������
    virtual void OnReadCompleted(CPF_UINT  ulConnectID, CPF_IBuff *pUserData);
    // һ�������ϵ�д�������
    virtual void OnWriteCompleted(CPF_UINT  ulConnectID, CPF_IBuff *pUserData);
protected:

private:
    CPF_DataPacketParse              *m_pPacketParse;
#ifdef WIN32
    CPF_IOCPServer                   *m_pIOCPServer;
#else
    CPF_EPOLLServer                  *m_pEpollServer;
#endif
};
#endif //__CPF_ManagerAllServer_H_


