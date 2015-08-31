#ifndef __CPF_Base_H_
#define __CPF_Base_H_
#include "CPF_typedef.h"

class CPF_Base
{
public:
    CPF_Base();
    virtual ~CPF_Base();

    /************************************************************************/
    /* 连接事件 */
    /************************************************************************/
    // 建立了一个新的连接
    virtual void OnConnectionEstablished(CPFDataPacket *pPacketData) = 0;
    // 一个连接关闭
    virtual void OnConnectionClosing(CPFDataPacket *pPacketData) = 0;
    // 在一个连接上发生了错误
    virtual void OnConnectionError(unsigned long lConnectID, int nError) = 0;
    // 一个连接上的读操作完成
    virtual void OnReadCompleted(CPFDataPacket *pPacketData) = 0;
    // 一个连接上的写操作完成
    virtual void OnWriteCompleted(CPFDataPacket *pPacketData) = 0;

    /************************************************************************/
    /*发送事件                                                              */
    /************************************************************************/
    virtual void SendToClient(CPFDataPacket *pPacketData) = 0;
};

#endif //__CPF_Base_H_
