#include "CPF_IManagerBUffer.h"

CPF_IManagerBUffer * CPF_IManagerBUffer::m_pInstance = 0;
CPF_IManagerBUffer::CPF_IManagerBUffer()
{
    InitializeCriticalSection(&m_mapIBufferSection);
}

CPF_IManagerBUffer::~CPF_IManagerBUffer()
{
    DeleteCriticalSection(&m_mapIBufferSection);
}

void CPF_IManagerBUffer::InitModule()
{

}

void CPF_IManagerBUffer::UnitModule()
{

}

CPF_IManagerBUffer * CPF_IManagerBUffer::GetInstance()
{
    if (m_pInstance == 0)
    {
        m_pInstance = new CPF_IManagerBUffer;
    }
    return m_pInstance;
}

bool CPF_IManagerBUffer::CPF_IManagerBufferADD(CPF_UINT uConnect, CPF_IBuff& IBuffer)
{
    EnterCriticalSection(&m_mapIBufferSection);
    for (map<CPF_UINT, CPF_IBuff>::iterator itemB = m_mapIBuffer.begin(); itemB != m_mapIBuffer.end(); itemB++)
    {
        if (uConnect == itemB->first)
        {
#ifdef CPF_TEST
            printf("Ersmap::size()= %d\n", m_mapIBuffer.size());
#endif
            delete itemB->second.m_pbuff;
            m_mapIBuffer.erase(itemB);
            break;
        }
    }
    m_mapIBuffer.insert(make_pair(uConnect, IBuffer));
#ifdef CPF_TEST
    printf("Addmap::size()= %d\n", m_mapIBuffer.size());
#endif
    LeaveCriticalSection(&m_mapIBufferSection);
    return true;
}


CPF_IBuff CPF_IManagerBUffer::CPF_IManagerBufferGET(CPF_UINT uConnect)
{
    CPF_IBuff IBuff;

    EnterCriticalSection(&m_mapIBufferSection);
    for (map<CPF_UINT, CPF_IBuff>::iterator itemB = m_mapIBuffer.begin(); itemB != m_mapIBuffer.end(); itemB++)
    {
        if (uConnect == itemB->first)
        {
            IBuff.m_pbuff = itemB->second.m_pbuff;
            IBuff.m_nLen = itemB->second.m_nLen;
            break;
        }
    }
    LeaveCriticalSection(&m_mapIBufferSection);
    return IBuff;
}

bool CPF_IManagerBUffer::CPF_IManagerBufferSUB(CPF_UINT uConnect)
{
    EnterCriticalSection(&m_mapIBufferSection);
    for (map<CPF_UINT, CPF_IBuff>::iterator itemB = m_mapIBuffer.begin(); itemB != m_mapIBuffer.end(); itemB++)
    {
        if (uConnect == itemB->first)
        {
            delete itemB->second.m_pbuff;
            m_mapIBuffer.erase(itemB);
#ifdef CPF_TEST
            printf("Submap::size()= %d\n", m_mapIBuffer.size());
#endif
            break;
        }
    }
    LeaveCriticalSection(&m_mapIBufferSection);
    return true;
}


CPF_IBuff CPF_IManagerBUffer::CPF_IManagerBufferPOP(CPF_UINT uConnect)
{
    CPF_IBuff IBuff;

    EnterCriticalSection(&m_mapIBufferSection);
    for (map<CPF_UINT, CPF_IBuff>::iterator itemB = m_mapIBuffer.begin(); itemB != m_mapIBuffer.end(); itemB++)
    {
        if (uConnect == itemB->first)
        {
            IBuff.m_pbuff = new char[itemB->second.m_nLen];
            IBuff.m_nLen = itemB->second.m_nLen;
            memcpy_s(IBuff.m_pbuff, itemB->second.m_nLen, itemB->second.m_pbuff, itemB->second.m_nLen);

            delete itemB->second.m_pbuff;
            m_mapIBuffer.erase(itemB);
#ifdef CPF_TEST
            printf("Popmap::size()= %d\n", m_mapIBuffer.size());
#endif
            break;
        }
    }
    LeaveCriticalSection(&m_mapIBufferSection);
    return IBuff;
}



