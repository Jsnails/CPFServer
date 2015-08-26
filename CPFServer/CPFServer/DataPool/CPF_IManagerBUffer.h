#ifndef __CPF_IManagerBUffer_H_
#define __CPF_IManagerBUffer_H_

#include "../common/CPF_IBuff.h"
#include "../common/CPF_typedef.h"
#include <windows.h>
#include <map>
using namespace std;

class CPF_IManagerBUffer 
{
public:
   static       CPF_IManagerBUffer *GetInstance();
   void InitModule();
   void UnitModule();

   bool         CPF_IManagerBufferADD(CPF_UINT uConnect, CPF_IBuff &pIBuffer);
   bool         CPF_IManagerBufferSUB(CPF_UINT uConnect);
   CPF_IBuff    CPF_IManagerBufferGET(CPF_UINT uConnect);
   CPF_IBuff    CPF_IManagerBufferPOP(CPF_UINT uConnect);
protected:
    CPF_IManagerBUffer();
    ~CPF_IManagerBUffer();
private:
    static CPF_IManagerBUffer   *m_pInstance;
    map<CPF_UINT, CPF_IBuff>    m_mapIBuffer;
    CRITICAL_SECTION            m_mapIBufferSection;
};

#endif //__CPF_IManagerBUffer_H_