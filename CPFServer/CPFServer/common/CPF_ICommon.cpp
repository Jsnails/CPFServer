#include "CPF_ICommon.h"

CPF_ICommon::CPF_ICommon()
{

}

CPF_ICommon::~CPF_ICommon()
{

}

bool CPF_ICommon::OnConnectionEstablished(CPF_UINT ulConnectID, CPF_IBuff *pUserData)
{
    return false;
}

void CPF_ICommon::OnConnectionClosing(CPF_UINT ulConnectID)
{

}

void CPF_ICommon::OnConnectionError(CPF_UINT ulConnectID, int nError)
{

}

void CPF_ICommon::OnReadCompleted(CPF_UINT ulConnectID, CPF_IBuff *pUserData)
{

}

void CPF_ICommon::OnWriteCompleted(CPF_UINT ulConnectID, CPF_IBuff *pUserData)
{

}
