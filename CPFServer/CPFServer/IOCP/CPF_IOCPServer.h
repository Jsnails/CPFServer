#ifndef __CPF_IOCPServer_H_
#define __CPF_IOCPServer_H_

#include "CPF_IOCP.H"
#include "../common/CPF_typedef.h"
#include <map>
using namespace std;

class CPF_Base;
class CPF_IOCPServer :public CIOCPServer
{ 
public:
    CPF_IOCPServer();
    ~CPF_IOCPServer();

    void InitModule(CPF_Base *pBase);
    void UnitModule();
    /************************************************************************/
    /*// �¼�֪ͨ����                                                       */
    /************************************************************************/
    // ������һ���µ�����
    void OnConnectionEstablished(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
    // һ�����ӹر�
    void OnConnectionClosing(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
    // ��һ�������Ϸ����˴���
    void OnConnectionError(CIOCPContext *pContext, CIOCPBuffer *pBuffer, int nError);
    // һ�������ϵĶ��������
    void OnReadCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
    // һ�������ϵ�д�������
    void OnWriteCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer);

    /************************************************************************/
    /*�����¼�                                                              */
    /************************************************************************/
    virtual void SendToClient(CPF_UINT lConnectID, const char* pBuffer, const long lLen);

protected:
    CIOCPContext *OnConnectManagerAdd(CIOCPContext *pContext);
    CIOCPContext *OnConnectManagerSub(CIOCPContext *pContext);
    CIOCPContext *OnConnectManagerFind(CPF_UINT ulConnectID);
    CPF_UINT      OnConnectIDFind(CIOCPContext *pContext);
private:
    CPF_UINT                         m_iConnectID;//���Ӽ���,Ҳ��������ϵͳ������ID
    map<CPF_UINT, CIOCPContext *>    m_mapIOCPContext; //
    CRITICAL_SECTION                 m_mapContextSection;
    CPF_Base                         *m_pManagerServer;
};

#endif __CPF_IOCPServer_H_