#include "CPF_EPOLL.h"
#include <pthread.h>
//#include "../common/"

CPF_EPOLL * CPF_EPOLL::m_pInstance = NULL;
CPF_EPOLL::CPF_EPOLL()
{
      m_iSerPort = 10090;
      m_iBackLogSize = 200;
      m_iBackStoreSize = 1024;
      m_iAcceptThreadId = 0;    //���������߳�ID
      m_iReadThreadId = 0;      //�������߳�ID
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
    int ret;            //��ʱ����,��ŷ���ֵ
    int epfd;           //�����õ�epoll
    int listenfd;      //����socket
    int connfd;        //���յ�������socket��ʱ����
    int i;            //��ʱ����,��ѯ������
    int nfds;        //��ʱ����,�ж��ٸ�socket���¼�

    struct epoll_event ev;                     //�¼���ʱ����
    struct epoll_event events[MAXEVENTS];      //�����¼�����
    socklen_t clilen;                          //����epoll_event�ṹ��ı���,ev����ע���¼�,�������ڻش�Ҫ������¼�
    struct sockaddr_in cliaddr;
    struct sockaddr_in svraddr;

    unsigned short uListenPort = 5000;
    int iBacklogSize = 200;
    int iBackStoreSize = 1024;

    struct CPF_PipeMsg msg;                        //��Ϣ��������

    //����epoll,��2.6.8�Ժ�İ汾,�������Ч,ֻҪ����0����ֵ����,�ں��Լ���̬����
    epfd = epoll_create(iBackStoreSize);
    if (epfd < 0)
    {
        cout << "AcceptThread, epoll_create fail:" << epfd << ",errno:" << errno << endl;

        OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, epfd);
        return NULL;
    }

    //��������socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        cout << "AcceptThread, socket fail:" << epfd << ",errno:" << errno << endl;

        close(epfd);

        OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, errno);
        return NULL;
    }

    //�Ѽ���socket����Ϊ��������ʽ
    OnSetNONblocking(listenfd);
    //���ü���socketΪ�˿�����
    OnSetReuseAddr(listenfd);

    //������Ҫ������¼���ص��ļ�������
    ev.data.fd = listenfd;
    //����Ҫ������¼�����
    ev.events = EPOLLIN | EPOLLET;
    //ע��epoll�¼�
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
    //����,׼����������
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
        //�ȴ�epoll�¼��ķ���,�����ǰ���źŵľ������������¼����������С,�������ֻ����´�epoll_waitʱ���,�¼����ᶪ
        nfds = epoll_wait(epfd, events, MAXEVENTS, 500);

        //�����������������¼�
        for (i = 0; i < nfds && pThis->m_bRun; ++i)
        {
            if (events[i].data.fd == listenfd)        //�Ǳ�����socket�ϵ��¼�
            {
                cout << "AcceptThread, events:" << events[i].events << ",errno:" << errno << endl;

                if (events[i].events&EPOLLIN)    //�����ӵ���
                {
                    do
                    {
                        clilen = sizeof(struct sockaddr);
                        connfd = accept(listenfd, (sockaddr *)&cliaddr, &clilen);
                        if (connfd > 0)
                        {
                            cout << "AcceptThread, accept:" << connfd << ",errno:" << errno << ",connect:" << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << endl;

                            //���ܵ�д����
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

                            if (errno == EAGAIN)    //û��������Ҫ������
                            {
                                break;
                            }
                            else if (errno == EINTR) //���ܱ��ж��źŴ��,,������֤�Է�����socket��δ�յ��˴���,Ӧ�ÿ���ʡ���ò��ж�
                            {
                                ;
                            }
                            else    //�������������Ϊ�������ֳ��ִ���,Ӧ�ùرպ����¼���
                            {

                                //��ʱ˵�����������Ѿ�������,��Ҫ���´����ͼ���
                                close(listenfd);
                                epoll_ctl(epfd, EPOLL_CTL_DEL, listenfd, &ev);

                                //��������socket
                                listenfd = socket(AF_INET, SOCK_STREAM, 0);
                                if (listenfd < 0)
                                {
                                    //cout << "AcceptThread, socket fail:" << epfd << ",errno:" << errno << endl;
                                    close(epfd);

                                    OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, errno);
                                    return NULL;
                                }

                                //�Ѽ���socket����Ϊ��������ʽ
                                OnSetNONblocking(listenfd);
                                //���ü���socketΪ�˿�����
                                OnSetReuseAddr(listenfd);

                                //������Ҫ������¼���ص��ļ�������
                                ev.data.fd = listenfd;
                                //����Ҫ������¼�����
                                ev.events = EPOLLIN | EPOLLET;
                                //ע��epoll�¼�
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
                                //����,׼����������
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
                else if (events[i].events&EPOLLERR || events[i].events&EPOLLHUP)    //���쳣����
                {
                    //��ʱ˵�����������Ѿ�������,��Ҫ���´����ͼ���
                    close(listenfd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, listenfd, &ev);

                    //��������socket
                    listenfd = socket(AF_INET, SOCK_STREAM, 0);
                    if (listenfd < 0)
                    {
                        cout << "AcceptThread, socket fail:" << epfd << ",errno:" << errno << endl;

                        close(epfd);

                        OnHandleIO(OP_ERROR, 0, NULL, 0, pThis, errno);
                        return NULL;
                    }

                    //�Ѽ���socket����Ϊ��������ʽ
                    OnSetNONblocking(listenfd);
                    //���ü���socketΪ�˿�����
                    OnSetReuseAddr(listenfd);

                    //������Ҫ������¼���ص��ļ�������
                    ev.data.fd = listenfd;
                    //����Ҫ������¼�����
                    ev.events = EPOLLIN | EPOLLET;
                    //ע��epoll�¼�
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
                    //����,׼����������
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

    //�رռ���������
    if (listenfd > 0)
    {
        close(listenfd);
    }
    //�رմ�����epoll
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

    int ret;        //��ʱ����,��ŷ���ֵ
    int epfd;        //�����õ�epoll
    int i;            //��ʱ����,��ѯ������
    int nfds;        //��ʱ����,�ж��ٸ�socket���¼�

    struct epoll_event ev;                     //�¼���ʱ����
    struct epoll_event events[MAXEVENTS];      //�����¼�����

    int iBackStoreSize = pThis->m_iBackStoreSize;

    char buf[MAXBUFSIZE];
    int    nread;                                            //�������ֽ���
    struct CPF_Ipport tIpPort;                               //��ַ�˿���Ϣ
    struct CPF_PeerInfo tPeerInfo;                           //�Է�������Ϣ
    map<int, struct CPF_Ipport> mIpPort;                     //socket��Ӧ�ĶԷ���ַ�˿���Ϣ
    map<int, struct CPF_Ipport>::iterator itIpPort;                      //��ʱ������
    map<struct CPF_Ipport, struct CPF_PeerInfo>::iterator itPeerInfo;    //��ʱ������

    struct CPF_PipeMsg msg;                                              //��Ϣ��������

    //����epoll,��2.6.8�Ժ�İ汾,�������Ч,ֻҪ����0����ֵ����,�ں��Լ���̬����
    epfd = epoll_create(iBackStoreSize);
    if (epfd < 0)
    {
        cout << "ReadThread, epoll_create fail:" << epfd << ",errno:" << errno << endl;
        return NULL;
    }

    while (pThis->m_bRun)
    {
        //�ӹܵ�������
        do
        {
            ret = read(pThis->m_ConnInfo.rfd, &msg, 14);
            if (ret > 0)
            {
                //�����е�fd��������Ч��
                if (ret == 14 && msg.fd > 0)
                {
                    if (msg.op == 1)    //�յ��µ�����
                    {
                        cout << "ReadThread,recv connect:" << msg.fd << ",errno:" << errno << endl;

                        //��socket����Ϊ��������ʽ
                        OnSetNONblocking(msg.fd);
                        //������������Ϣ�������±���Ϣ
                        ev.data.fd = msg.fd;
                        //��������ע��Ķ������¼�
                        ev.events = EPOLLIN | EPOLLET;
                        //ע��ev
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
                    else if (msg.op == 2)    //�Ͽ�ĳ������
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

        //�ȴ�epoll�¼��ķ���,�����ǰ���źŵľ������������¼����������С,�������ֻ����´�epoll_waitʱ���,�¼����ᶪ
        nfds = epoll_wait(epfd, events, MAXEVENTS, 500);

        //�����������������¼�
        for (i = 0; i < nfds && pThis->m_bRun; ++i)
        {
            //cout << "ReadThread, events:" << events[i].events << ",errno:" << errno << endl;
            if (events[i].events&EPOLLIN)   //�����ݿɶ�
            {
                do
                {
                    bzero(buf, MAXBUFSIZE);
                    nread = read(events[i].data.fd, buf, MAXBUFSIZE);
                    if (nread > 0)    //��������
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
                    else if (nread < 0) //��ȡʧ��
                    {
                        if (errno == EAGAIN)    //û��������
                        {
                            //cout << "ReadThread, read:" << nread << ",errno:" << errno << ",no data" << endl;
                            break;
                        }
                        else if (errno == EINTR) //���ܱ��ڲ��ж��źŴ��,������֤�Է�����socket��δ�յ��˴���,Ӧ�ÿ���ʡ���ò��ж�
                        {
                            //cout << "ReadThread, read:" << nread << ",errno:" << errno << ",interrupt" << endl;
                        }
                        else    //�ͻ��������ر�
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
                    else if (nread == 0) //�ͻ��������ر�
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
            else if (events[i].events&EPOLLERR || events[i].events&EPOLLHUP)    //���쳣����
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

    //�ر���������
    for (itIpPort = mIpPort.begin(); itIpPort != mIpPort.end(); itIpPort++)
    {
        if (itIpPort->first > 0)
        {
            close(itIpPort->first);
        }
    }
    //�رմ�����epoll
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
    int fd[2];                    //��д�ܵ�

    //Ϊ��Ӧ�ó��򲻱ض�����ϵͳ���õ�errno��EINTR���,���Բ�ȡ���ַ�ʽ:1.�����ж��ź�,2.�����ж��ź�
    //1.��signal()������װ���źŴ������ϵͳĬ�ϻ��Զ����������жϵ�ϵͳ���ã����������������أ�
    //  ����Ӧ�ó��򲻱ض�����ϵͳ���õ�errno��EINTR��飬������Զ�����������.
    //2.��sigaction()��Ĭ�϶����ǲ��Զ����������жϵ�ϵͳ���ã�
    //  ������������ʹ��sigaction()ʱ��Ҫ�Զ����������жϵ�ϵͳ���ã�����Ҫʹ��sigaction��SA_RESTARTѡ��
    //�����ź�
    //sigset_t newmask;
    //sigemptyset(&newmask);
    //sigaddset(&newmask, SIGINT);
    //sigaddset(&newmask, SIGUSR1);
    //sigaddset(&newmask, SIGUSR2);
    //sigaddset(&newmask, SIGQUIT);
    //pthread_sigmask(SIG_BLOCK, &newmask, NULL);
    //�����ź�
    //Ĭ���Զ����������жϵ�ϵͳ����,����������������,Ӧ�ó��򲻱ض�����ϵͳ���õ�errno��EINTR���
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

    //����Ϊ����״̬
    m_bRun = true;

    //�����ܵ�
    ret = pipe(fd);
    if (ret < 0)
    {
        //cout << "main, pipe fail:" << ret << ",errno:" << errno << endl;
        m_bRun = false;
        return false;
    }
    m_ConnInfo.rfd = fd[0];
    m_ConnInfo.wfd = fd[1];

    //��������Ϊ��������ʽ
    OnSetNONblocking(m_ConnInfo.rfd);

    //�����߳�ʱ���õĲ���
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);                   //���ð󶨵��߳�,�Ի�ȡ�ϸߵ���Ӧ�ٶ�
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);        //���÷�����߳�

    //�������������߳�
    ret = pthread_create(&m_iAcceptThreadId, &attr, AcceptThread, this);
    if (ret != 0)
    {
        cout << "main, pthread_create AcceptThread fail:" << ret << ",errno:" << errno << endl;

        m_bRun = false;
        close(m_ConnInfo.rfd);
        close(m_ConnInfo.wfd);

        return false;
    }

    //�������������߳�
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

