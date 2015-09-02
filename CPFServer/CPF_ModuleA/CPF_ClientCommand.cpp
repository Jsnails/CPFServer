#include "CPF_ClientCommand.h"
#include <stdio.h>

CPF_ClientCommand * CPF_ClientCommand::m_pInstance = NULL;

CPF_ClientCommand::CPF_ClientCommand()
{
}


CPF_ClientCommand::~CPF_ClientCommand()
{
}

int CPF_ClientCommand::DoMessage(CPF_UINT ulConnectID, CPF_IPacketData *pUserData)
{

    return ulConnectID;
}

CPF_ClientCommand * CPF_ClientCommand::GetInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new CPF_ClientCommand;
    }
    return m_pInstance;
}

void CPF_ClientCommand::DelInstance()
{
    if (m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

bool CPF_ClientCommand::SetManagerModuleObject(CPF_ManagerChildModule * pModule)
{


    return true;
}
