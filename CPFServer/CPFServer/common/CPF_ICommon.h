#ifndef __CPF_ICommon_H_
#define __CPF_ICommon_H_
#include "CPF_Base.h"

class  CPF_ICommon : public CPF_Base
{
public:
    CPF_ICommon();
    ~CPF_ICommon();

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

};
#endif //__CPF_ICommon_H_
