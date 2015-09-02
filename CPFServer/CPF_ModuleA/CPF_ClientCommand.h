#pragma once

#include "CPF_BaseCommand.h"
#include "../CPFServer/ChildModule/CPF_ManagerChildModule.h"

class CPF_ClientCommand : public CPF_BaseCommand
{
public:
    ~CPF_ClientCommand();
    static CPF_ClientCommand *GetInstance();
    static void               DelInstance();

    bool SetManagerModuleObject(CPF_ManagerChildModule * pModule);
    int  DoMessage(CPF_UINT  ulConnectID, CPF_IPacketData *pUserData);
protected:
    CPF_ClientCommand();

private:
    static CPF_ClientCommand *m_pInstance;
};

