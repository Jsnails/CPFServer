#pragma once

#include "../CPFServer/common/CPF_IPacketData.h"
#include "../CPFServer/common/CPF_typedef.h"

class CPF_BaseCommand
{
public:

    /************************************************************************/
    /*�ͻ����¼�*/
    /************************************************************************/
    virtual int DoMessage(CPF_UINT  ulConnectID, CPF_IPacketData *pUserData) = 0;
};

