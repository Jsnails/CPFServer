#ifndef __CPF_ManagerAllServer_H_
#define __CPF_ManagerAllServer_H_
#ifdef WIN32
#include "./IOCP/CPF_IOCPServer.h"
#else
#include "./EPOLL/CPF_EPOLLServer.h"
#endif
#include "common/CPF_Base.h"
#include "ChildModule/CPF_ManagerChildModule.h"

class CPF_DataPacketParse;
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
    /* 连接事件 */
    /************************************************************************/
    // 建立了一个新的连接
    virtual bool OnConnectionEstablished(CPF_UINT  ulConnectID, CPF_IBuff *pUserData);
    // 一个连接关闭
    virtual void OnConnectionClosing(CPF_UINT  ulConnectID);
    // 在一个连接上发生了错误
    virtual void OnConnectionError(CPF_UINT  ulConnectID, int nError);
    // 一个连接上的读操作完成
    virtual void OnReadCompleted(CPF_UINT  ulConnectID, CPF_IBuff *pUserData);
    // 一个连接上的写操作完成
    virtual void OnWriteCompleted(CPF_UINT  ulConnectID, CPF_IBuff *pUserData);
protected:

private:
    CPF_DataPacketParse              *m_pPacketParse;
#ifdef WIN32
    CPF_IOCPServer                   *m_pIOCPServer;
#else
    CPF_EPOLLServer                  *m_pEpollServer;
#endif

    CPF_ManagerChildModule            m_childManagerModule;
};

#endif //__CPF_ManagerAllServer_H_


