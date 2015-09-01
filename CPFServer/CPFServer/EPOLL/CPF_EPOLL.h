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

//管道消息结构
struct CPF_PipeMsg
{
    int op;
    int fd;
    unsigned int     ip;
    unsigned short port;
};

//地址端口结构
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

//对应于对方地址端口的连接信息
struct CPF_PeerInfo
{
    int fd;                  //对应连接句柄
    unsigned int contime;    //最后连接时间
    unsigned int rcvtime;    //收到数据时间
    unsigned int rcvbyte;    //收到字节个数
    unsigned int sndtime;    //发送数据时间
    unsigned int sndbyte;    //发送字节个数
};

//连接结构
struct CPF_ConnectInfo
{
    int rfd;                                            //管道读端
    int wfd;                                            //管道写端
    map<struct CPF_Ipport, struct CPF_PeerInfo> peer;   //对方信息
};

//传输数据结构
struct CPF_EPOLLBuffer
{
    char * pBuff;
    int    ilen;
};

#define  MAXBUFSIZE  1024 * 12                       //读数据缓冲区大小
#define  MAXEVENTS   1024                            //最大事件数

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
    // 建立了一个新的连接
    virtual void OnConnectionEstablished(CPF_UINT  iConnectID, CPF_EPOLLBuffer *pbuff);
    // 一个连接关闭
    virtual void OnConnectionClosing(CPF_UINT  iConnectID, CPF_EPOLLBuffer *pbuff);
    // 在一个连接上发生了错误
    virtual void OnConnectionError(CPF_UINT  iConnectID, int nError);
    // 一个连接上的读操作完成
    virtual void OnReadCompleted(CPF_UINT  iConnectID, CPF_EPOLLBuffer *pbuff);
    // 一个连接上的写操作完成
    virtual void OnWriteCompleted(CPF_UINT  iConnectID, CPF_EPOLLBuffer *pbuff);
protected:
    static void   Sig_Pro(int signum);
    static void * AcceptThread(void *pUser);
    static void * ReadThread(void *pUser);//读数据线程
protected:
    static void OnSetNONblocking(int iSocket);
    static void OnSetReuseAddr(int iSocket);
private:
    static void OnHandleIO(CPF_UINT iType, CPF_UINT iConnect, char *pBuff, int iLength, CPF_EPOLL *pUser,int iError = -1);

private:
    bool m_bRun;                        //运行标志
    CPF_ConnectInfo m_ConnInfo;         //连接信息对象
    int  m_iSerPort;                    //服务端口
    int  m_iBackLogSize;                //监听个数
    int  m_iBackStoreSize;              //Epoll创建大小
    pthread_t m_iAcceptThreadId;        //接收连接线程ID
    pthread_t m_iReadThreadId;          //读数据线程ID
    static CPF_EPOLL *m_pInstance;
};

#endif //__CPF_EPOLL_H_
