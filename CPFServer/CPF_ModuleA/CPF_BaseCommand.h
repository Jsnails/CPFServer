#pragma once

#include "../CPFServer/common/CPF_IPacketData.h"
#include "../CPFServer/common/CPF_typedef.h"

class CPF_BaseCommand
{
public:

    /************************************************************************/
    /*客户端事件*/
    /************************************************************************/
    virtual int DoMessage(CPF_UINT  ulConnectID, CPF_IPacketData *pUserData) = 0;
};

