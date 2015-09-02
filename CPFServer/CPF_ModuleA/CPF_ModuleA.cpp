// CPF_ModuleA.cpp : 定义 DLL 应用程序的导出函数。
//
#define MODULE_A_DLL

#ifdef WIN32
#ifdef MODULE_A_DLL
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif
#else
#define DECLDIR
#endif

#include "CPF_ClientCommand.h"
#include "../CPFServer/common/CPF_IPacketData.h"

extern "C"
{
    DECLDIR int CPF_LoadModule(CPF_ManagerChildModule * pModule);
    DECLDIR int CPF_UNLoadModule();
    DECLDIR const char* CPF_GetModuleName();
    DECLDIR const char* CPF_GetModuleKey();
    DECLDIR int  CPF_DoMessage(CPF_UINT ulConnectID, CPF_IPacketData *pUserData);
    DECLDIR int  CPF_DoModuleMessage();
    DECLDIR bool CPF_GetModuleState();
}

DECLDIR int CPF_LoadModule(CPF_ManagerChildModule * pModule)
{
    CPF_ClientCommand::GetInstance()->SetManagerModuleObject(pModule);

    return 1;
}

DECLDIR int CPF_UNLoadModule()
{
    CPF_ClientCommand::DelInstance();
    return 0;
}

DECLDIR const char* CPF_GetModuleName()
{
    return "ModuleA";
}

DECLDIR const char* CPF_GetModuleKey()
{
    return "2E9AC47C7F0A4E04A76169E3CC6D6B3C";
}

DECLDIR int CPF_DoModuleMessage()
{
    return 1;
}

DECLDIR bool CPF_GetModuleState()
{
    return true;
}

DECLDIR int CPF_DoMessage(CPF_UINT ulConnectID, CPF_IPacketData *pUserData)
{
   return CPF_ClientCommand::GetInstance()->DoMessage(ulConnectID, pUserData);
}


