#include "CPF_DataPacketParse.h"
#include "../common/CPF_typedef.h"

CPF_DataPacketParse::CPF_DataPacketParse()
{

}

CPF_DataPacketParse::~CPF_DataPacketParse()
{

}

bool CPF_DataPacketParse::OneConnect(CPF_UINT ulConnectID, CPF_IBuff *pUserData)
{
    //�����ӵ������ڴ˴�����֤����
    if (ulConnectID == 1)
    {
        return false;
    }
    //��֤�ɹ�,�������ӹ�������
    return true;
}

void CPF_DataPacketParse::OneDisConnect(CPF_UINT ulConnectID)
{
    //���ӶϿ����ӹ��������Ƴ�

}

