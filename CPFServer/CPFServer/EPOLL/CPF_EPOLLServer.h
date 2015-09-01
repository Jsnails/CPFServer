#ifndef  __CPF_EPOLLServer_H_
#define  __CPF_EPOLLServer_H_
#include "CPF_EPOLL.h"
#include "../common/CPF_typedef.h"
#include <pthread.h>

class CPF_DataPacketParse;
class CPF_Base;
class CPF_EPOLLServer : public CPF_EPOLL
{
public:
    CPF_EPOLLServer();
    ~CPF_EPOLLServer();

    void InitModule(CPF_Base *pBase);
    void UnitModule();
    /************************************************************************/
    /*// �¼�֪ͨ����                                                       */
    /************************************************************************/
    // ������һ���µ�����
    virtual void OnConnectionEstablished(CPF_UINT  iConnectID, CPF_EPOLLBuffer *pbuff);
    // һ�����ӹر�
    virtual void OnConnectionClosing(CPF_UINT  iConnectID, CPF_EPOLLBuffer *pbuff);
    // ��һ�������Ϸ����˴���
    virtual void OnConnectionError(CPF_UINT  iConnectID, int nError);
    // һ�������ϵĶ��������
    virtual void OnReadCompleted(CPF_UINT  iConnectID, CPF_EPOLLBuffer *pbuff);
    // һ�������ϵ�д�������
    virtual void OnWriteCompleted(CPF_UINT  iConnectID, CPF_EPOLLBuffer *pbuff);

    /************************************************************************/
    /*�����¼�                                                              */
    /************************************************************************/
    virtual void SendToClient(CPF_UINT lConnectID, const char* pBuffer, const long lLen);

protected:
    CPF_UINT      OnConnectManagerAdd(CPF_UINT ulConnectID);
    CPF_UINT      OnConnectManagerSub(CPF_UINT ulConnectID);
    CPF_UINT      OnConnectManagerFind(CPF_UINT ulConnectID);//�ҵ���Ӧ��socket���Ӿ��
    CPF_UINT      OnConnectIDFind(CPF_UINT ulConnectID);     //�ҵ�ϵͳ���ɵ�ID
private:
    CPF_UINT                         m_iConnectID;              //���Ӽ���,Ҳ��������ϵͳ������ID
    map<CPF_UINT,CPF_UINT>           m_mapEpollContext;
    pthread_mutex_t                  m_mapEpollMutex;
    CPF_Base                         *m_pManagerServer;

};

#endif //__CPF_EPOLLServer_H_
