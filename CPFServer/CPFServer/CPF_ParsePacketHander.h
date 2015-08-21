#pragma once
#include "./common/CPF_typedef.h"
#include "./CPFDataPacketParse/CPF_ParseDataPacketBase.h"

class CPF_ParsePacketHander : public CPF_ParseDataPacketBase
{
public:
    CPF_ParsePacketHander();
    ~CPF_ParsePacketHander();

    // ������һ���µ�����
    void OnConnectionEstablished(CPFDataPacket *pPacketData);
    // һ�����ӹر�
    void OnConnectionClosing(CPFDataPacket *pPacketData);
    // ��һ�������Ϸ����˴���
    void OnConnectionError(unsigned long lConnectID, int nError);
    // һ�������ϵĶ��������
    void OnReadCompleted(CPFDataPacket *pPacketData);
    // һ�������ϵ�д�������
    void OnWriteCompleted(CPFDataPacket *pPacketData);

    /************************************************************************/
    /*�����¼�                                                              */
    /************************************************************************/
    void SendToClient(CPFDataPacket *pPacketData);
};

