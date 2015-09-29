#ifndef __CPF_ManagerChildModule_H_
#define __CPF_ManagerChildModule_H_

#include "../common/CPF_IPacketData.h"
#include "../common/CPF_typedef.h"
#include <vector>
using namespace std;

class CPF_ManagerChildModule;
typedef int(_stdcall  *PCPF_LoadModule)(CPF_ManagerChildModule * pModule);
typedef int(_stdcall  *PCPF_UNLoadModule)();
typedef const char*(_stdcall *PCPF_GetModuleName)();
typedef const char*(_stdcall *PCPF_GetModuleKey)();
typedef int(_stdcall  *PCPF_DoMessage)(CPF_UINT ulConnectID, CPF_IPacketData *pUserData);

struct CPF_ChildModuleInfo
{
    void *             hModule;
    PCPF_LoadModule    LoadModule;
    PCPF_UNLoadModule  UNLoadModule;
    PCPF_GetModuleName GetModuleName;
    PCPF_GetModuleKey  GetModuleKey;
    PCPF_DoMessage     DoMessage;
    vector<CPF_UINT>   vecCommand;
};

class CPF_ManagerChildModule
{
public:
    CPF_ManagerChildModule();
    ~CPF_ManagerChildModule();

    void InitModule();
    void UnitModule();
private:
    vector<CPF_ChildModuleInfo> m_vecManagerModule;
};

#endif  //__CPF_ManagerChildModule_H_