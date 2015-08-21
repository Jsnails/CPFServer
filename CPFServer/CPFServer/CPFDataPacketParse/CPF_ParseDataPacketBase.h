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

#ifdef WIN32
class CPF_DLL_EXPORT CPF_ParseDataPacketBase
#else
class CPF_ParseDataPacketBase
#endif
{
public:
    CPF_ParseDataPacketBase();
    virtual ~CPF_ParseDataPacketBase();
};

#endif




