#ifndef __CPF_ParseDataPacketBase_H_
#define  __CPF_ParseDataPacketBase_H_

#ifdef WIN32
#if defined CPF_PARSEPACKET_BUILD
#define CPF_DLL_EXPORT __declspec(dllexport)
#else
#define CPF_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define CPF_DLL_EXPORT
#endif 

#include "../common/CPF_IBuff.h"
#include "../common/CPF_typedef.h"
#include "../DataPool/CPF_IManagerBUffer.h"

#ifdef WIN32
class CPF_DLL_EXPORT CPF_ParseDataPacketBase
#else
class CPF_ParseDataPacketBase
#endif
{
public:
    CPF_ParseDataPacketBase();
    virtual ~CPF_ParseDataPacketBase();

    virtual bool OneConnect(CPF_UINT  ulConnectID, CPF_IBuff *pUserData) = 0;
    virtual void OneDisConnect(CPF_UINT ulConnectID) = 0;
    virtual bool CheckRecvPacketParse(CPF_IManagerBUffer *pIManagerBuff, CPF_UINT uConnectID, CPF_IBuff *pBuffer) = 0;
    virtual CPF_PACKETMODULE  GetPacketModule() = 0;
protected:
    CPF_PACKETMODULE m_iTransModule; //数据传输模型
};

#endif




