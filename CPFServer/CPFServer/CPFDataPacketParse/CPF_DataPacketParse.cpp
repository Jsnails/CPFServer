#include "CPF_DataPacketParse.h"
#include "../common/CPF_typedef.h"
#include "../DataPool/CPF_IManagerBUffer.h"

CPF_DataPacketParse::CPF_DataPacketParse()
{
    m_iTransModule = CPF_GETSTREAM; //此处使用流模式
}

CPF_DataPacketParse::~CPF_DataPacketParse()
{

}

bool CPF_DataPacketParse::OneConnect(CPF_UINT ulConnectID, CPF_IBuff *pUserData)
{
    //有连接到来，在此处做验证处理
    /* if (ulConnectID == 1)
     {
     return false;
     }*/
    //验证成功,加入连接管理缓存区


    return true;
}

void CPF_DataPacketParse::OneDisConnect(CPF_UINT ulConnectID)
{
    //连接断开，从管理缓存区移除

}

bool CPF_DataPacketParse::CheckRecvPacketParse(CPF_IManagerBUffer *pIManagerBuff, CPF_UINT uConnectID, CPF_IBuff *pBuffer)
{
    if (pIManagerBuff == NULL)
    {
        return false;
    }
    bool bIsComplete = false;
    CPF_IBuff Ibuffer;
    Ibuffer.m_pbuff = new char[pBuffer->m_nLen + 1];
    memset(Ibuffer.m_pbuff, 0, pBuffer->m_nLen + 1);
    Ibuffer.m_nLen = pBuffer->m_nLen;
    memcpy(Ibuffer.m_pbuff, pBuffer->m_pbuff, pBuffer->m_nLen);

    int iLen = 0;
    iLen = pBuffer->m_nLen;

    if (Ibuffer.m_pbuff[0] == '#' && Ibuffer.m_pbuff[1] == '#' && Ibuffer.m_pbuff[2] == '#' && Ibuffer.m_pbuff[3] == '#')
    {//包头

        if (Ibuffer.m_pbuff[iLen - 1] == '#' && Ibuffer.m_pbuff[iLen - 2] == '#' && Ibuffer.m_pbuff[iLen - 3] == '#' && Ibuffer.m_pbuff[iLen - 4] == '#')
        {//完整包
            pIManagerBuff->CPF_IManagerBufferADD(uConnectID, Ibuffer);//加入缓存区
            bIsComplete = true;
        }
        else
        {//不完整包,组包
            pIManagerBuff->CPF_IManagerBufferADD(uConnectID, Ibuffer);//加入缓存区
        }
    }
    else //后续包到达
    {
        if (Ibuffer.m_pbuff[iLen - 1] == '#' && Ibuffer.m_pbuff[iLen - 2] == '#' && Ibuffer.m_pbuff[iLen - 3] == '#' && Ibuffer.m_pbuff[iLen - 4] == '#')
        {//完整包

            CPF_IBuff IBuffTmp;

            CPF_IBuff IBuffPop = pIManagerBuff->CPF_IManagerBufferPOP(uConnectID);
            IBuffPop.m_pbuff[IBuffPop.m_nLen] = '\0';
            int iLength = pBuffer->m_nLen + IBuffPop.m_nLen + 1;
            IBuffTmp.m_pbuff = new char[iLength];
            memset(IBuffTmp.m_pbuff, 0, iLength);

            strncpy(IBuffTmp.m_pbuff, IBuffPop.m_pbuff,IBuffPop.m_nLen);
            strncat(IBuffTmp.m_pbuff, Ibuffer.m_pbuff,Ibuffer.m_nLen);

            IBuffTmp.m_nLen = pBuffer->m_nLen + IBuffPop.m_nLen;
            pIManagerBuff->CPF_IManagerBufferADD(uConnectID, IBuffTmp);

            bIsComplete = true;
        }
        else
        {//依然不完整,这时候就被视为恶意攻击数据包,丢掉该包
           
        }
    }

    return bIsComplete;
}

CPF_PACKETMODULE CPF_DataPacketParse::GetPacketModule()
{
    return m_iTransModule;
}

