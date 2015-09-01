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
    /* 连接事件 */
    /************************************************************************/
    // 建立了一个新的连接
    virtual bool OnConnectionEstablished(CPF_UINT  ulConnectID, CPF_IBuff *pUserData) = 0;
    // 一个连接关闭
    virtual void OnConnectionClosing(CPF_UINT  ulConnectID) = 0;
    // 在一个连接上发生了错误
    virtual void OnConnectionError(CPF_UINT  ulConnectID, int nError) = 0;
    // 一个连接上的读操作完成
    virtual void OnReadCompleted(CPF_UINT  ulConnectID, CPF_IBuff *pUserData) = 0;
    // 一个连接上的写操作完成
    virtual void OnWriteCompleted(CPF_UINT  ulConnectID, CPF_IBuff *pUserData) = 0;
};

#endif //__CPF_Base_H_
