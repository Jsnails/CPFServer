#pragma once
#include "./common/CPF_typedef.h"
#include "./CPFDataPacketParse/CPF_ParseDataPacketBase.h"

class CPF_ParsePacketHander : public CPF_ParseDataPacketBase
{
public:
    CPF_ParsePacketHander();
    ~CPF_ParsePacketHander();

    // 建立了一个新的连接
    void OnConnectionEstablished(CPFDataPacket *pPacketData);
    // 一个连接关闭
    void OnConnectionClosing(CPFDataPacket *pPacketData);
    // 在一个连接上发生了错误
    void OnConnectionError(unsigned long lConnectID, int nError);
    // 一个连接上的读操作完成
    void OnReadCompleted(CPFDataPacket *pPacketData);
    // 一个连接上的写操作完成
    void OnWriteCompleted(CPFDataPacket *pPacketData);

    /************************************************************************/
    /*发送事件                                                              */
    /************************************************************************/
    void SendToClient(CPFDataPacket *pPacketData);
};

