#include "CPF_DataPacketParse.h"
#include "../common/CPF_typedef.h"

CPF_DataPacketParse::CPF_DataPacketParse()
{

}

CPF_DataPacketParse::~CPF_DataPacketParse()
{

}

bool CPF_DataPacketParse::OneConnect(CPF_UINT ulConnectID, CPF_IBuff *pUserData)
{
    //有连接到来，在此处做验证处理
    if (ulConnectID == 1)
    {
        return false;
    }
    //验证成功,加入连接管理缓存区
    return true;
}

void CPF_DataPacketParse::OneDisConnect(CPF_UINT ulConnectID)
{
    //连接断开，从管理缓存区移除

}

