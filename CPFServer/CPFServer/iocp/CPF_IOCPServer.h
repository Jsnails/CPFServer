#ifndef __CPF_IOCPServer_H_
#define __CPF_IOCPServer_H_

#include "CPF_IOCP.H"

class CPF_IOCPServer :public CIOCPServer
{ 
public:
    CPF_IOCPServer();
    ~CPF_IOCPServer();

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
    virtual void SendToClient(unsigned long lConnectID, const char* pBuffer, const long lLen);
};

#endif __CPF_IOCPServer_H_