#ifndef __CPF_ICommon_H_
#define __CPF_ICommon_H_
#include "CPF_Base.h"

class  CPF_ICommon : public CPF_Base
{
public:
    CPF_ICommon();
    ~CPF_ICommon();

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

};
#endif //__CPF_ICommon_H_
