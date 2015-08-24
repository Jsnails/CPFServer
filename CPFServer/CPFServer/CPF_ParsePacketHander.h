#pragma once
#include "./common/CPF_typedef.h"
#include "./CPFDataPacketParse/CPF_ParseDataPacketBase.h"

class CPF_ParsePacketHander : public CPF_ParseDataPacketBase
{
public:
    CPF_ParsePacketHander();
    ~CPF_ParsePacketHander();

    bool OneConnect(unsigned long  ulConnectID, CPF_IBuff *pUserData);//返回false，关闭客户端连接
    void OneDisConnect(unsigned long ulConnectID);
};

