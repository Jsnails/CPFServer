#ifndef __CPF_IOCPServer_H_
#define __CPF_IOCPServer_H_

#include "CPF_IOCP.H"
#include "../common/CPF_typedef.h"
#include <map>
using namespace std;

class CPF_Base;
class CPF_IOCPServer :public CIOCPServer
{ 
public:
    CPF_IOCPServer();
    ~CPF_IOCPServer();

    void InitModule(CPF_Base *pBase);
    void UnitModule();
    /************************************************************************/
    /*// 事件通知函数                                                       */
    /************************************************************************/
    // 建立了一个新的连接
    void OnConnectionEstablished(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
    // 一个连接关闭
    void OnConnectionClosing(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
    // 在一个连接上发生了错误
    void OnConnectionError(CIOCPContext *pContext, CIOCPBuffer *pBuffer, int nError);
    // 一个连接上的读操作完成
    void OnReadCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
    // 一个连接上的写操作完成
    void OnWriteCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer);

    /************************************************************************/
    /*发送事件                                                              */
    /************************************************************************/
    virtual void SendToClient(CPF_UINT lConnectID, const char* pBuffer, const long lLen);

protected:
    CIOCPContext *OnConnectManagerAdd(CIOCPContext *pContext);
    CIOCPContext *OnConnectManagerSub(CIOCPContext *pContext);
    CIOCPContext *OnConnectManagerFind(CPF_UINT ulConnectID);
    CPF_UINT      OnConnectIDFind(CIOCPContext *pContext);
private:
    CPF_UINT                         m_iConnectID;//连接计数,也就是整个系统的连接ID
    map<CPF_UINT, CIOCPContext *>    m_mapIOCPContext; //
    CRITICAL_SECTION                 m_mapContextSection;
    CPF_Base                         *m_pManagerServer;
};

#endif __CPF_IOCPServer_H_