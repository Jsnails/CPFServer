#ifndef  __CPF_EPOLLServer_H_
#define  __CPF_EPOLLServer_H_
#include "CPF_EPOLL.h"
#include "../common/CPF_typedef.h"

class CPF_DataPacketParse;
class CPF_EPOLLServer : public CPF_EPOLL
{
public:
    CPF_EPOLLServer();
    ~CPF_EPOLLServer();

    void InitModule();
    void UnitModule();
    /************************************************************************/
    /*// 事件通知函数                                                       */
    /************************************************************************/
    // 建立了一个新的连接
    virtual void OnConnectionEstablished(CPF_UINT  iConnectID, CPF_EPOLLBuffer *pbuff);
    // 一个连接关闭
    virtual void OnConnectionClosing(CPF_UINT  iConnectID, CPF_EPOLLBuffer *pbuff);
    // 在一个连接上发生了错误
    virtual void OnConnectionError(CPF_UINT  iConnectID, int nError);
    // 一个连接上的读操作完成
    virtual void OnReadCompleted(CPF_UINT  iConnectID, CPF_EPOLLBuffer *pbuff);
    // 一个连接上的写操作完成
    virtual void OnWriteCompleted(CPF_UINT  iConnectID, CPF_EPOLLBuffer *pbuff);

    /************************************************************************/
    /*发送事件                                                              */
    /************************************************************************/
    virtual void SendToClient(CPF_UINT lConnectID, const char* pBuffer, const long lLen);

protected:
    CPF_UINT      OnConnectManagerAdd(CPF_UINT ulConnectID);
    CPF_UINT      OnConnectManagerSub(CPF_UINT ulConnectID);
    CPF_UINT      OnConnectManagerFind(CPF_UINT ulConnectID);
private:
    CPF_UINT                         m_iConnectID;              //连接计数,也就是整个系统的连接ID
    CPF_DataPacketParse              *m_pPacketParse;
    map<CPF_UINT,CPF_UINT>           m_mapIOCPContext;
};

#endif //__CPF_EPOLLServer_H_
