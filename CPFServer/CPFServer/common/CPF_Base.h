#ifndef __CPF_Base_H_
#define __CPF_Base_H_
#include "CPF_typedef.h"

class CPF_Base
{
public:
    CPF_Base();
    virtual ~CPF_Base();

    /************************************************************************/
    /* �����¼� */
    /************************************************************************/
    // ������һ���µ�����
    virtual void OnConnectionEstablished(CPFDataPacket *pPacketData) = 0;
    // һ�����ӹر�
    virtual void OnConnectionClosing(CPFDataPacket *pPacketData) = 0;
    // ��һ�������Ϸ����˴���
    virtual void OnConnectionError(unsigned long lConnectID, int nError) = 0;
    // һ�������ϵĶ��������
    virtual void OnReadCompleted(CPFDataPacket *pPacketData) = 0;
    // һ�������ϵ�д�������
    virtual void OnWriteCompleted(CPFDataPacket *pPacketData) = 0;

    /************************************************************************/
    /*�����¼�                                                              */
    /************************************************************************/
    virtual void SendToClient(CPFDataPacket *pPacketData) = 0;
};

#endif //__CPF_Base_H_
