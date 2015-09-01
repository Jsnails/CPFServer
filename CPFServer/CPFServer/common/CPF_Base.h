#ifndef __CPF_Base_H_
#define __CPF_Base_H_
#include "CPF_typedef.h"
#include "CPF_IBuff.h"

class CPF_Base
{
public:
    CPF_Base();
    virtual ~CPF_Base();

    /************************************************************************/
    /* �����¼� */
    /************************************************************************/
    // ������һ���µ�����
    virtual bool OnConnectionEstablished(CPF_UINT  ulConnectID, CPF_IBuff *pUserData) = 0;
    // һ�����ӹر�
    virtual void OnConnectionClosing(CPF_UINT  ulConnectID) = 0;
    // ��һ�������Ϸ����˴���
    virtual void OnConnectionError(CPF_UINT  ulConnectID, int nError) = 0;
    // һ�������ϵĶ��������
    virtual void OnReadCompleted(CPF_UINT  ulConnectID, CPF_IBuff *pUserData) = 0;
    // һ�������ϵ�д�������
    virtual void OnWriteCompleted(CPF_UINT  ulConnectID, CPF_IBuff *pUserData) = 0;
};

#endif //__CPF_Base_H_
