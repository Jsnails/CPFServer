#ifndef __CPF_DataPacketParse_H_
#define __CPF_DataPacketParse_H_

#ifdef WIN32
#if defined CPF_PARSEPACKET_BUILD
#define CPF_DLL_EXPORT __declspec(dllexport)
#else
#define CPF_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define CPF_DLL_EXPORT
#endif 

#include "CPF_ParseDataPacketBase.h"

class CPF_DLL_EXPORT CPF_DataPacketParse :public CPF_ParseDataPacketBase
{
public:
    CPF_DataPacketParse();
    virtual ~CPF_DataPacketParse();

    bool OneConnect(CPF_UINT  ulConnectID, CPF_IBuff *pUserData);//����false���رտͻ������ӣ��˺�����Ϊ�û�������֤ʹ�á�
    void OneDisConnect(CPF_UINT ulConnectID);
};
#endif 