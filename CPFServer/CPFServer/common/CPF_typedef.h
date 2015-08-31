#ifndef __CPF_TYPEDEF_H_
#define __CPF_TYPEDEF_H_

typedef unsigned int CPF_UINT;
typedef unsigned long CPF_ULONG;

enum CPF_OP_SERVER
{
    OP_ACCEPT = 1,
    OP_WRITE,
    OP_READ,
    OP_DISCONN,
    OP_ERROR,
    OP_MAX,
};

//#define CPF_TEST   //��������ģʽ

struct CPFDataPacket
{
    unsigned long lConnectID;
    char *pBuffer;
    long lLen;
    CPFDataPacket()
    {
        lConnectID = 0;
        pBuffer = 0;
        lLen = 0;
    }
};

enum CPF_PACKETMODULE
{
    CPF_GETSTREAM = 1,  //������ģʽ
    CPF_GETHEAD,        //���ݰ�ģʽ
    CPF_MODULEMAX,
};

#ifdef WIN32

#define BUFFER_SIZE 1024*12		    // I/O����Ļ�������С
#define MAX_THREAD	2			    // I/O�����̵߳�����

#else


#endif

#endif //__CPF_TYPEDEF_H_
