#ifndef __CPF_TYPEDEF_H_
#define __CPF_TYPEDEF_H_

#define OP_ACCEPT	1
#define OP_WRITE	2
#define OP_READ		3
#define BUFFER_SIZE 1024*12		    // I/O请求的缓冲区大小
#define MAX_THREAD	2			    // I/O服务线程的数量

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

#endif __CPF_TYPEDEF_H_
