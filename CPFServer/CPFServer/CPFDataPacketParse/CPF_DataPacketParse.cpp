#include "CPF_DataPacketParse.h"
#include "../common/CPF_typedef.h"
#include "../DataPool/CPF_IManagerBUffer.h"

CPF_DataPacketParse::CPF_DataPacketParse()
{
    m_iTransModule = CPF_GETSTREAM; //�˴�ʹ����ģʽ
}

CPF_DataPacketParse::~CPF_DataPacketParse()
{

}

bool CPF_DataPacketParse::OneConnect(CPF_UINT ulConnectID, CPF_IBuff *pUserData)
{
    //�����ӵ������ڴ˴�����֤����
    /* if (ulConnectID == 1)
     {
     return false;
     }*/
    //��֤�ɹ�,�������ӹ�������


    return true;
}

void CPF_DataPacketParse::OneDisConnect(CPF_UINT ulConnectID)
{
    //���ӶϿ����ӹ��������Ƴ�

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
    {//��ͷ

        if (Ibuffer.m_pbuff[iLen - 1] == '#' && Ibuffer.m_pbuff[iLen - 2] == '#' && Ibuffer.m_pbuff[iLen - 3] == '#' && Ibuffer.m_pbuff[iLen - 4] == '#')
        {//������
            pIManagerBuff->CPF_IManagerBufferADD(uConnectID, Ibuffer);//���뻺����
            bIsComplete = true;
        }
        else
        {//��������,���
            pIManagerBuff->CPF_IManagerBufferADD(uConnectID, Ibuffer);//���뻺����
        }
    }
    else //����������
    {
        if (Ibuffer.m_pbuff[iLen - 1] == '#' && Ibuffer.m_pbuff[iLen - 2] == '#' && Ibuffer.m_pbuff[iLen - 3] == '#' && Ibuffer.m_pbuff[iLen - 4] == '#')
        {//������

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
        {//��Ȼ������,��ʱ��ͱ���Ϊ���⹥�����ݰ�,�����ð�
           
        }
    }

    return bIsComplete;
}

CPF_PACKETMODULE CPF_DataPacketParse::GetPacketModule()
{
    return m_iTransModule;
}

