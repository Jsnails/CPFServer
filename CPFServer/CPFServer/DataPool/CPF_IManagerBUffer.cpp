#include "CPF_IManagerBUffer.h"

CPF_IManagerBUffer * CPF_IManagerBUffer::m_pInstance = 0;
CPF_IManagerBUffer::CPF_IManagerBUffer()
{
#ifdef WIN32
    InitializeCriticalSection(&m_mapIBufferSection);
#else
    m_mapIBuffMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&m_mapIBuffMutex,NULL);
#endif

}

CPF_IManagerBUffer::~CPF_IManagerBUffer()
{
#ifdef WIN32
    DeleteCriticalSection(&m_mapIBufferSection);
#else
    pthread_mutex_destroy(&m_mapIBuffMutex);
#endif

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
#ifdef WIN32
    EnterCriticalSection(&m_mapIBufferSection);
#else
    pthread_mutex_lock(&m_mapIBuffMutex);
#endif

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

#ifdef WIN32
    LeaveCriticalSection(&m_mapIBufferSection);
#else
    pthread_mutex_unlock(&m_mapIBuffMutex);
#endif

    return true;
}


CPF_IBuff CPF_IManagerBUffer::CPF_IManagerBufferGET(CPF_UINT uConnect)
{
    CPF_IBuff IBuff;
#ifdef WIN32
    EnterCriticalSection(&m_mapIBufferSection);
#else
    pthread_mutex_lock(&m_mapIBuffMutex);
#endif

    for (map<CPF_UINT, CPF_IBuff>::iterator itemB = m_mapIBuffer.begin(); itemB != m_mapIBuffer.end(); itemB++)
    {
        if (uConnect == itemB->first)
        {
            IBuff.m_pbuff = itemB->second.m_pbuff;
            IBuff.m_nLen = itemB->second.m_nLen;
            break;
        }
    }

#ifdef WIN32
    LeaveCriticalSection(&m_mapIBufferSection);
#else
    pthread_mutex_unlock(&m_mapIBuffMutex);
#endif

    return IBuff;
}

bool CPF_IManagerBUffer::CPF_IManagerBufferSUB(CPF_UINT uConnect)
{
#ifdef WIN32
    EnterCriticalSection(&m_mapIBufferSection);
#else
    pthread_mutex_lock(&m_mapIBuffMutex);
#endif

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

#ifdef WIN32
    LeaveCriticalSection(&m_mapIBufferSection);
#else
    pthread_mutex_unlock(&m_mapIBuffMutex);
#endif
    return true;
}


CPF_IBuff CPF_IManagerBUffer::CPF_IManagerBufferPOP(CPF_UINT uConnect)
{
    CPF_IBuff IBuff;

#ifdef WIN32
    EnterCriticalSection(&m_mapIBufferSection);
#else
    pthread_mutex_lock(&m_mapIBuffMutex);
#endif

    for (map<CPF_UINT, CPF_IBuff>::iterator itemB = m_mapIBuffer.begin(); itemB != m_mapIBuffer.end(); itemB++)
    {
        if (uConnect == itemB->first)
        {
            IBuff.m_pbuff = new char[itemB->second.m_nLen];
            IBuff.m_nLen = itemB->second.m_nLen;
            memcpy(IBuff.m_pbuff, itemB->second.m_pbuff, itemB->second.m_nLen);

            delete itemB->second.m_pbuff;
            m_mapIBuffer.erase(itemB);

#ifdef CPF_TEST
            printf("Popmap::size()= %d\n", m_mapIBuffer.size());
#endif
            break;
        }
    }
#ifdef WIN32
    LeaveCriticalSection(&m_mapIBufferSection);
#else
    pthread_mutex_unlock(&m_mapIBuffMutex);
#endif

    return IBuff;
}



