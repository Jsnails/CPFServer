#ifndef __CPF_EPOLL_H_
#define __CPF_EPOLL_H_
#include <deque>
#include <map>
#include <vector>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cctype>
#include <sstream>
#include <utility>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "../common/CPF_typedef.h"
using namespace std;

#pragma pack(1)

//�ܵ���Ϣ�ṹ
struct CPF_PipeMsg
{
    int op;
    int fd;
    unsigned int     ip;
    unsigned short port;
};

//��ַ�˿ڽṹ
struct CPF_Ipport
{
    unsigned int ip;
    unsigned short port;
    bool operator < (const CPF_Ipport rhs) const
    {
        return (ip < rhs.ip || (ip == rhs.ip && port < rhs.port));
    }
    bool operator == (const CPF_Ipport rhs) const
    {
        return (ip == rhs.ip && port == rhs.port);
    }
};

//��Ӧ�ڶԷ���ַ�˿ڵ�������Ϣ
struct CPF_PeerInfo
{
    int fd;                  //��Ӧ���Ӿ��
    unsigned int contime;    //�������ʱ��
    unsigned int rcvtime;    //�յ�����ʱ��
    unsigned int rcvbyte;    //�յ��ֽڸ���
    unsigned int sndtime;    //��������ʱ��
    unsigned int sndbyte;    //�����ֽڸ���
};

//���ӽṹ
struct CPF_ConnectInfo
{
    int rfd;                                            //�ܵ�����
    int wfd;                                            //�ܵ�д��
    map<struct CPF_Ipport, struct CPF_PeerInfo> peer;   //�Է���Ϣ
};

//�������ݽṹ
struct CPF_EPOLLBuffer
{
    char * pBuff;
    int    ilen;
};

#define  MAXBUFSIZE  1024 * 12                       //�����ݻ�������С
#define  MAXEVENTS   1024                            //����¼���

#pragma pack()

class CPF_EPOLL
{
public:
    CPF_EPOLL();
    ~CPF_EPOLL();

    bool StartServer(int iSerPort = 10090,int BackLogSize = 500,int BackStoreSize = 1024);
    void EndServer();

    void OnCloseAconnect(CPF_UINT uConnect);
    void SendText(CPF_UINT iConnID, char *pData, int iLength);
protected:
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
protected:
    static void   Sig_Pro(int signum);
    static void * AcceptThread(void *pUser);
    static void * ReadThread(void *pUser);//�������߳�
protected:
    static void OnSetNONblocking(int iSocket);
    static void OnSetReuseAddr(int iSocket);
private:
    static void OnHandleIO(CPF_UINT iType, CPF_UINT iConnect, char *pBuff, int iLength, CPF_EPOLL *pUser,int iError = -1);

private:
    bool m_bRun;                        //���б�־
    CPF_ConnectInfo m_ConnInfo;         //������Ϣ����
    int  m_iSerPort;                    //����˿�
    int  m_iBackLogSize;                //��������
    int  m_iBackStoreSize;              //Epoll������С
    pthread_t m_iAcceptThreadId;        //���������߳�ID
    pthread_t m_iReadThreadId;          //�������߳�ID
    static CPF_EPOLL *m_pInstance;
};

#endif //__CPF_EPOLL_H_
