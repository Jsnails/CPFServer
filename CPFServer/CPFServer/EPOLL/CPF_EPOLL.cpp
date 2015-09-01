#include "CPF_EPOLL.h"
#include <pthread.h>
//#include "../common/"

CPF_EPOLL * CPF_EPOLL::m_pInstance = NULL;
CPF_EPOLL::CPF_EPOLL()
{
      m_iSerPort = 10090;
      m_iBackLogSize = 200;
      m_iBackStoreSize = 1024;
      m_iAcceptThreadId = 0;    //接收连接线程ID
      m_iReadThreadId = 0;      //读数据线程ID
}

CPF_EPOLL::~CPF_EPOLL()
{

}

void CPF_EPOLL::OnSetNONblocking(int iSocket)
{
    int opts;
    opts = fcntl(iSocket, F_GETFL);
    if (opts < 0)
    {
        perror("fcntl(iSocket,GETFL)");
        exit(1);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(iSocket, F_SETFL, opts) < 0)
    {
        perror("fcntl(iSocket,SETFL,opts)");
        exit(1);
    }
}

void CPF_EPOLL::OnSetReuseAddr(int iSocket)
{
    int opt;
    opt = 1;
    if (setsockopt(iSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(&opt)) < 0)
    {
        perror("setsockopt");
    }
}

void CPF_EPOLL::Sig_Pro(int signum)
{
    if (signum == SIGQUIT)
    {
        if(m_pInstance != NULL)
        m_pInstance->m_bRun = false;
    }
}

void * CPF_EPOLL::AcceptThread(void *pUser)
{
    CPF_EPOLL *pThis = (CPF_EPOLL*)pUser;
    int ret;            //临时变量,存放返回值
    int epfd;           //监听用的epoll
    int listenfd;      //监听socket
    int connfd;        //接收到的连接socket临时变量
    int i;            //临时变量,轮询数组用
    int nfds;        //临时变量,有多少个socket有事件

    struct epoll_event ev;                     //事件临时变量
    struct epoll_event events[MAXEVENTS];      //监听事件数组
    socklen_t clilen;                          //声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
    struct sockaddr_in cliaddr;
    struct sockaddr_in svraddr;

    unsigned short uListenPort = 5000;
    int iBacklogSize = 200;
    int iBackStoreSize = 1024;

    struct CPF_PipeMsg msg;                        //消息队列数据

    //创建epoll,对2.6.8以后的版本,其参数无效,只要大于0的数值就行,内核自己动态分配
    epfd = epoll_create(iBackStoreSize);
    if (epfd < 0)
    {
        cout << "AcceptThread, epoll_create fail:" << epfd << ",errno:" << errno << endl;

        OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, epfd);
        return NULL;
    }

    //创建监听socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        cout << "AcceptThread, socket fail:" << epfd << ",errno:" << errno << endl;

        close(epfd);

        OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, errno);
        return NULL;
    }

    //把监听socket设置为非阻塞方式
    OnSetNONblocking(listenfd);
    //设置监听socket为端口重用
    OnSetReuseAddr(listenfd);

    //设置与要处理的事件相关的文件描述符
    ev.data.fd = listenfd;
    //设置要处理的事件类型
    ev.events = EPOLLIN | EPOLLET;
    //注册epoll事件
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
    if (ret != 0)
    {
        cout << "AcceptThread, epoll_ctl fail:" << ret << ",errno:" << errno << endl;

        close(listenfd);
        close(epfd);

        OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, ret);
        return NULL;
    }

    bzero(&svraddr, sizeof(svraddr));
    svraddr.sin_family = AF_INET;
    svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    svraddr.sin_port = htons(uListenPort);
    bind(listenfd, (sockaddr *)&svraddr, sizeof(svraddr));
    //监听,准备接收连接
    ret = listen(listenfd, iBacklogSize);
    if (ret != 0)
    {
        cout << "AcceptThread, listen fail:" << ret << ",errno:" << errno << endl;

        close(listenfd);
        close(epfd);

        OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, ret);
        return NULL;
    }

    while (pThis->m_bRun)
    {
        //等待epoll事件的发生,如果当前有信号的句柄数大于输出事件数组的最大大小,超过部分会在下次epoll_wait时输出,事件不会丢
        nfds = epoll_wait(epfd, events, MAXEVENTS, 500);

        //处理所发生的所有事件
        for (i = 0; i < nfds && pThis->m_bRun; ++i)
        {
            if (events[i].data.fd == listenfd)        //是本监听socket上的事件
            {
                cout << "AcceptThread, events:" << events[i].events << ",errno:" << errno << endl;

                if (events[i].events&EPOLLIN)    //有连接到来
                {
                    do
                    {
                        clilen = sizeof(struct sockaddr);
                        connfd = accept(listenfd, (sockaddr *)&cliaddr, &clilen);
                        if (connfd > 0)
                        {
                            cout << "AcceptThread, accept:" << connfd << ",errno:" << errno << ",connect:" << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << endl;

                            //往管道写数据
                            msg.op = 1;
                            msg.fd = connfd;
                            msg.ip = cliaddr.sin_addr.s_addr;
                            msg.port = cliaddr.sin_port;
                            ret = write(pThis->m_ConnInfo.wfd, &msg, 14);
                            if (ret != 14)
                            {
                                cout << "AcceptThread, write fail:" << ret << ",errno:" << errno << endl;

                                close(connfd);

                                OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, errno);
                            }
                        }
                        else
                        {
                            cout << "AcceptThread, accept:" << connfd << ",errno:" << errno << endl;

                            if (errno == EAGAIN)    //没有连接需要接收了
                            {
                                break;
                            }
                            else if (errno == EINTR) //可能被中断信号打断,,经过验证对非阻塞socket并未收到此错误,应该可以省掉该步判断
                            {
                                ;
                            }
                            else    //其它情况可以认为该描述字出现错误,应该关闭后重新监听
                            {

                                //此时说明该描述字已经出错了,需要重新创建和监听
                                close(listenfd);
                                epoll_ctl(epfd, EPOLL_CTL_DEL, listenfd, &ev);

                                //创建监听socket
                                listenfd = socket(AF_INET, SOCK_STREAM, 0);
                                if (listenfd < 0)
                                {
                                    //cout << "AcceptThread, socket fail:" << epfd << ",errno:" << errno << endl;
                                    close(epfd);

                                    OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, errno);
                                    return NULL;
                                }

                                //把监听socket设置为非阻塞方式
                                OnSetNONblocking(listenfd);
                                //设置监听socket为端口重用
                                OnSetReuseAddr(listenfd);

                                //设置与要处理的事件相关的文件描述符
                                ev.data.fd = listenfd;
                                //设置要处理的事件类型
                                ev.events = EPOLLIN | EPOLLET;
                                //注册epoll事件
                                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
                                if (ret != 0)
                                {
                                    //cout << "AcceptThread, epoll_ctl fail:" << ret << ",errno:" << errno << endl;
                                    close(listenfd);
                                    close(epfd);

                                    OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, errno);
                                    return NULL;
                                }

                                bzero(&svraddr, sizeof(svraddr));
                                svraddr.sin_family = AF_INET;
                                svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
                                svraddr.sin_port = htons(uListenPort);
                                bind(listenfd, (sockaddr *)&svraddr, sizeof(svraddr));
                                //监听,准备接收连接
                                ret = listen(listenfd, iBacklogSize);
                                if (ret != 0)
                                {
                                    cout << "AcceptThread, listen fail:" << ret << ",errno:" << errno << endl;

                                    close(listenfd);
                                    close(epfd);

                                    OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, errno);
                                    return NULL;
                                }
                            }
                        }
                    } while (pThis->m_bRun);
                }
                else if (events[i].events&EPOLLERR || events[i].events&EPOLLHUP)    //有异常发生
                {
                    //此时说明该描述字已经出错了,需要重新创建和监听
                    close(listenfd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, listenfd, &ev);

                    //创建监听socket
                    listenfd = socket(AF_INET, SOCK_STREAM, 0);
                    if (listenfd < 0)
                    {
                        cout << "AcceptThread, socket fail:" << epfd << ",errno:" << errno << endl;

                        close(epfd);

                        OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, errno);
                        return NULL;
                    }

                    //把监听socket设置为非阻塞方式
                    OnSetNONblocking(listenfd);
                    //设置监听socket为端口重用
                    OnSetReuseAddr(listenfd);

                    //设置与要处理的事件相关的文件描述符
                    ev.data.fd = listenfd;
                    //设置要处理的事件类型
                    ev.events = EPOLLIN | EPOLLET;
                    //注册epoll事件
                    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
                    if (ret != 0)
                    {
                        cout << "AcceptThread, epoll_ctl fail:" << ret << ",errno:" << errno << endl;

                        close(listenfd);
                        close(epfd);

                        OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, errno);
                        return NULL;
                    }

                    bzero(&svraddr, sizeof(svraddr));
                    svraddr.sin_family = AF_INET;
                    svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
                    svraddr.sin_port = htons(uListenPort);
                    bind(listenfd, (sockaddr *)&svraddr, sizeof(svraddr));
                    //监听,准备接收连接
                    ret = listen(listenfd, iBacklogSize);
                    if (ret != 0)
                    {
                        cout << "AcceptThread, listen fail:" << ret << ",errno:" << errno << endl;

                        close(listenfd);
                        close(epfd);

                        OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, errno);
                        return NULL;
                    }
                }
            }
        }
    }

    //关闭监听描述字
    if (listenfd > 0)
    {
        close(listenfd);
    }
    //关闭创建的epoll
    if (epfd > 0)
    {
        close(epfd);
    }

    cout << "AcceptThread, exit" << endl;

    return NULL;
}

void * CPF_EPOLL::ReadThread(void *pUser)
{
    CPF_EPOLL *pThis = (CPF_EPOLL*)pUser;

    int ret;        //临时变量,存放返回值
    int epfd;        //连接用的epoll
    int i;            //临时变量,轮询数组用
    int nfds;        //临时变量,有多少个socket有事件

    struct epoll_event ev;                     //事件临时变量
    struct epoll_event events[MAXEVENTS];      //监听事件数组

    int iBackStoreSize = pThis->m_iBackStoreSize;

    char buf[MAXBUFSIZE];
    int    nread;                                            //读到的字节数
    struct CPF_Ipport tIpPort;                               //地址端口信息
    struct CPF_PeerInfo tPeerInfo;                           //对方连接信息
    map<int, struct CPF_Ipport> mIpPort;                     //socket对应的对方地址端口信息
    map<int, struct CPF_Ipport>::iterator itIpPort;                      //临时迭代子
    map<struct CPF_Ipport, struct CPF_PeerInfo>::iterator itPeerInfo;    //临时迭代子

    struct CPF_PipeMsg msg;                                              //消息队列数据

    //创建epoll,对2.6.8以后的版本,其参数无效,只要大于0的数值就行,内核自己动态分配
    epfd = epoll_create(iBackStoreSize);
    if (epfd < 0)
    {
        cout << "ReadThread, epoll_create fail:" << epfd << ",errno:" << errno << endl;
        return NULL;
    }

    while (pThis->m_bRun)
    {
        //从管道读数据
        do
        {
            ret = read(pThis->m_ConnInfo.rfd, &msg, 14);
            if (ret > 0)
            {
                //队列中的fd必须是有效的
                if (ret == 14 && msg.fd > 0)
                {
                    if (msg.op == 1)    //收到新的连接
                    {
                        cout << "ReadThread,recv connect:" << msg.fd << ",errno:" << errno << endl;

                        //把socket设置为非阻塞方式
                        OnSetNONblocking(msg.fd);
                        //设置描述符信息和数组下标信息
                        ev.data.fd = msg.fd;
                        //设置用于注测的读操作事件
                        ev.events = EPOLLIN | EPOLLET;
                        //注册ev
                        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, msg.fd, &ev);
                        if (ret != 0)
                        {
                            //cout << "ReadThread, epoll_ctl fail:" << ret << ",errno:" << errno << endl;
                            close(msg.fd);
                            OnHandleIO(OP_ERROR, msg.fd, NULL, 0, pThis, errno);
                        }
                        else
                        {
                            mIpPort[msg.fd] = tIpPort;
                            tPeerInfo.fd = msg.fd;
                            tPeerInfo.contime = time(NULL);
                            tPeerInfo.rcvtime = 0;
                            tPeerInfo.rcvbyte = 0;
                            tPeerInfo.sndtime = 0;
                            tPeerInfo.sndbyte = 0;
                            pThis->m_ConnInfo.peer[tIpPort] = tPeerInfo;

                            bzero(buf, MAXBUFSIZE);
                            nread = read(msg.fd, buf, MAXBUFSIZE);
                            if(nread > 0)
                            {
                               //cout << "ReadThread, recv connect:" << msg.fd << ",OnConnectionEstablished:" << buf << endl;

                                OnHandleIO(OP_ACCEPT, msg.fd,buf, nread, pThis);
                            }
                            else
                            {
                                close(msg.fd);
                            }
                        }
                    }
                    else if (msg.op == 2)    //断开某个连接
                    {
                       //cout << "ReadThread, recv close:" << msg.fd << ",errno:" << errno << endl;

                        close(msg.fd);
                        epoll_ctl(epfd, EPOLL_CTL_DEL, msg.fd, &ev);

                        itIpPort = mIpPort.find(msg.fd);
                        if (itIpPort != mIpPort.end())
                        {
                            mIpPort.erase(itIpPort);

                            itPeerInfo = pThis->m_ConnInfo.peer.find(itIpPort->second);
                            if (itPeerInfo != pThis->m_ConnInfo.peer.end())
                            {
                                pThis->m_ConnInfo.peer.erase(itPeerInfo);
                            }
                        }

                        OnHandleIO(OP_DISCONN, msg.fd, NULL, 0, pThis);
                    }
                }
            }
            else
            {
                break;
            }
        } while (pThis->m_bRun);

        //等待epoll事件的发生,如果当前有信号的句柄数大于输出事件数组的最大大小,超过部分会在下次epoll_wait时输出,事件不会丢
        nfds = epoll_wait(epfd, events, MAXEVENTS, 500);

        //处理所发生的所有事件
        for (i = 0; i < nfds && pThis->m_bRun; ++i)
        {
            //cout << "ReadThread, events:" << events[i].events << ",errno:" << errno << endl;
            if (events[i].events&EPOLLIN)   //有数据可读
            {
                do
                {
                    bzero(buf, MAXBUFSIZE);
                    nread = read(events[i].data.fd, buf, MAXBUFSIZE);
                    if (nread > 0)    //读到数据
                    {
                        cout << "ReadThread, read:" << nread << ",errno:" << errno << endl;

                        itIpPort = mIpPort.find(events[i].data.fd);
                        if (itIpPort != mIpPort.end())
                        {
                            itPeerInfo = pThis->m_ConnInfo.peer.find(itIpPort->second);
                            if (itPeerInfo != pThis->m_ConnInfo.peer.end())
                            {
                                itPeerInfo->second.rcvtime = time(NULL);
                                itPeerInfo->second.rcvbyte += nread;
                            }
                        }
                        OnHandleIO(OP_READ, events[i].data.fd, buf, nread, pThis);
                    }
                    else if (nread < 0) //读取失败
                    {
                        if (errno == EAGAIN)    //没有数据了
                        {
                            //cout << "ReadThread, read:" << nread << ",errno:" << errno << ",no data" << endl;
                            break;
                        }
                        else if (errno == EINTR) //可能被内部中断信号打断,经过验证对非阻塞socket并未收到此错误,应该可以省掉该步判断
                        {
                            //cout << "ReadThread, read:" << nread << ",errno:" << errno << ",interrupt" << endl;
                        }
                        else    //客户端主动关闭
                        {
                            cout << "ReadThread, read:" << nread << ",errno:" << errno << ",peer error" << endl;

                            close(events[i].data.fd);
                            epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                            itIpPort = mIpPort.find(events[i].data.fd);
                            if (itIpPort != mIpPort.end())
                            {
                                mIpPort.erase(itIpPort);

                                itPeerInfo =pThis->m_ConnInfo.peer.find(itIpPort->second);
                                if (itPeerInfo != pThis->m_ConnInfo.peer.end())
                                {
                                    pThis->m_ConnInfo.peer.erase(itPeerInfo);
                                }
                            }

                            OnHandleIO(OP_DISCONN, events[i].data.fd, NULL, 0, pThis);
                            break;
                        }
                    }
                    else if (nread == 0) //客户端主动关闭
                    {
                        //cout << "ReadThread, read:" << nread << ",errno:" << errno << ",peer close" << endl;

                        close(events[i].data.fd);
                        epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                        itIpPort = mIpPort.find(events[i].data.fd);
                        if (itIpPort != mIpPort.end())
                        {
                            mIpPort.erase(itIpPort);

                            itPeerInfo = pThis->m_ConnInfo.peer.find(itIpPort->second);
                            if (itPeerInfo != pThis->m_ConnInfo.peer.end())
                            {
                                pThis->m_ConnInfo.peer.erase(itPeerInfo);
                            }
                        }

                        OnHandleIO(OP_DISCONN, events[i].data.fd, NULL, 0, pThis);
                        break;
                    }
                } while (pThis->m_bRun);
            }
            else if (events[i].events&EPOLLERR || events[i].events&EPOLLHUP)    //有异常发生
            {
                cout << "ReadThread, read:" << nread << ",errno:" << errno << ",err or hup" << endl;

                close(events[i].data.fd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                itIpPort = mIpPort.find(events[i].data.fd);
                if (itIpPort != mIpPort.end())
                {
                    mIpPort.erase(itIpPort);

                    itPeerInfo = pThis->m_ConnInfo.peer.find(itIpPort->second);
                    if (itPeerInfo != pThis->m_ConnInfo.peer.end())
                    {
                        pThis->m_ConnInfo.peer.erase(itPeerInfo);
                    }
                }

                OnHandleIO(OP_ERROR, msg.fd, NULL, 0, pThis, errno);
            }
        }
    }

    //关闭所有连接
    for (itIpPort = mIpPort.begin(); itIpPort != mIpPort.end(); itIpPort++)
    {
        if (itIpPort->first > 0)
        {
            close(itIpPort->first);
        }
    }
    //关闭创建的epoll
    if (epfd > 0)
    {
        close(epfd);
    }

    cout << "ReadThread, exit" << endl;

    return NULL;
}

bool CPF_EPOLL::StartServer(int iSerPort, int BackLogSize /*= 500*/, int BackStoreSize /*= 1024*/)
{
    m_iSerPort = iSerPort;
    m_iBackLogSize = BackLogSize;
    m_iBackStoreSize = BackStoreSize;

    int ret;
    int fd[2];                    //读写管道

    //为让应用程序不必对慢速系统调用的errno做EINTR检查,可以采取两种方式:1.屏蔽中断信号,2.处理中断信号
    //1.由signal()函数安装的信号处理程序，系统默认会自动重启动被中断的系统调用，而不是让它出错返回，
    //  所以应用程序不必对慢速系统调用的errno做EINTR检查，这就是自动重启动机制.
    //2.对sigaction()的默认动作是不自动重启动被中断的系统调用，
    //  因此如果我们在使用sigaction()时需要自动重启动被中断的系统调用，就需要使用sigaction的SA_RESTART选项
    //忽略信号
    //sigset_t newmask;
    //sigemptyset(&newmask);
    //sigaddset(&newmask, SIGINT);
    //sigaddset(&newmask, SIGUSR1);
    //sigaddset(&newmask, SIGUSR2);
    //sigaddset(&newmask, SIGQUIT);
    //pthread_sigmask(SIG_BLOCK, &newmask, NULL);
    //处理信号
    //默认自动重启动被中断的系统调用,而不是让它出错返回,应用程序不必对慢速系统调用的errno做EINTR检查
    //signal(SIGINT, sig_pro);
    //signal(SIGUSR1, sig_pro);
    //signal(SIGUSR2, sig_pro);
    //signal(SIGQUIT, sig_pro);

    //struct sigaction sa;
    //sa.sa_flags = SA_RESTART;
    //sa.sa_handler =  Sig_Pro;
    //sigaction(SIGINT, &sa, NULL);
    //sigaction(SIGUSR1, &sa, NULL);
    //sigaction(SIGUSR2, &sa, NULL);
    //sigaction(SIGQUIT, &sa, NULL);

    //设置为运行状态
    m_bRun = true;

    //创建管道
    ret = pipe(fd);
    if (ret < 0)
    {
        //cout << "main, pipe fail:" << ret << ",errno:" << errno << endl;
        m_bRun = false;
        return false;
    }
    m_ConnInfo.rfd = fd[0];
    m_ConnInfo.wfd = fd[1];

    //读端设置为非阻塞方式
    OnSetNONblocking(m_ConnInfo.rfd);

    //创建线程时采用的参数
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);                   //设置绑定的线程,以获取较高的响应速度
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);        //设置分离的线程

    //创建接收连接线程
    ret = pthread_create(&m_iAcceptThreadId, &attr, AcceptThread, this);
    if (ret != 0)
    {
        cout << "main, pthread_create AcceptThread fail:" << ret << ",errno:" << errno << endl;

        m_bRun = false;
        close(m_ConnInfo.rfd);
        close(m_ConnInfo.wfd);

        return false;
    }

    //创建接收连接线程
    ret = pthread_create(&m_iReadThreadId, &attr, ReadThread, this);
    if (ret != 0)
    {
        cout << "main, pthread_create ReadThread fail:" << ret << ",errno:" << errno << endl;

        m_bRun = false;
        pthread_join(m_iAcceptThreadId, NULL);
        close(m_ConnInfo.rfd);
        close(m_ConnInfo.wfd);

        return false;
    }
    return true;
}

void CPF_EPOLL::EndServer()
{
    m_bRun = false;
    pthread_join(m_iAcceptThreadId, NULL);
    pthread_join(m_iReadThreadId, NULL);
    close(m_ConnInfo.rfd);
    close(m_ConnInfo.wfd);
}

void CPF_EPOLL::OnHandleIO(CPF_UINT iType, CPF_UINT iConnect, char *pBuff, int iLength, CPF_EPOLL *pUser, int iError)
{
    if (!pUser)
    {
        return;
    }

    CPF_EPOLLBuffer *pCPFbuff = NULL;
    if (iLength > 0)
    {
        pCPFbuff = new CPF_EPOLLBuffer;
        pCPFbuff->ilen = iLength;
        pCPFbuff->pBuff = new char[iLength + 1];
        memset(pCPFbuff->pBuff, 0, iLength + 1);
        memcpy(pCPFbuff->pBuff, pBuff, iLength);
    }

    switch (iType)
    {
        case OP_ACCEPT:
        {
                          pUser->OnConnectionEstablished(iConnect, pCPFbuff);
        }
        break;
        case OP_READ:
        {
                        pUser->OnReadCompleted(iConnect, pCPFbuff);
        }
        break;
        case OP_DISCONN:
        {
                           pUser->OnConnectionClosing(iConnect, pCPFbuff);
        }
        break;
        case OP_ERROR:
        {
                         pUser->OnConnectionError(iConnect, iError);
        }
        break;
        default:
        break;
    }

    if (pCPFbuff != NULL)
   {
        delete pCPFbuff->pBuff;
        pCPFbuff->pBuff = NULL;
        delete pCPFbuff;
        pCPFbuff = NULL;
   }
}

void CPF_EPOLL::SendText(CPF_UINT iConnID, char *pData, int iLength)
{
    send(iConnID, pData, iLength);
}


void CPF_EPOLL::OnConnectionEstablished(CPF_UINT iConnectID, CPF_EPOLLBuffer *pbuff)
{

}

void CPF_EPOLL::OnConnectionClosing(CPF_UINT iConnectID, CPF_EPOLLBuffer *pbuff)
{

}

void CPF_EPOLL::OnConnectionError(CPF_UINT iConnectID, int nError)
{

}

void CPF_EPOLL::OnReadCompleted(CPF_UINT iConnectID, CPF_EPOLLBuffer *pbuff)
{

}

void CPF_EPOLL::OnWriteCompleted(CPF_UINT iConnectID, CPF_EPOLLBuffer *pbuff)
{

}

void CPF_EPOLL::OnCloseAconnect(CPF_UINT uConnect)
{
    close(uConnect);
}

