#include "CPF_ManagerChildModule.h"

#ifdef WIN32
#include <stdio.h> 
#include <windows.h> 
#else

#endif

CPF_ManagerChildModule::CPF_ManagerChildModule()
{

}

CPF_ManagerChildModule::~CPF_ManagerChildModule()
{

}

void CPF_ManagerChildModule::InitModule()
{
    //∂ØÃ¨º”‘ÿdll
    CPF_ChildModuleInfo module;

#ifdef WIN32
    module.hModule = LoadLibraryA("CPF_ModuleA.dll");
    module.DoMessage = (PCPF_DoMessage)GetProcAddress((HMODULE)module.hModule, "CPF_DoMessage");
#else

#endif
    m_vecManagerModule.push_back(module);
}

void CPF_ManagerChildModule::UnitModule()
{
    //–∂‘ÿ∂ØÃ¨dll

}
