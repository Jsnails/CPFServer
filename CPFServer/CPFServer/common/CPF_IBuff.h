#ifndef __CPF_IBuff_H_
#define __CPF_IBuff_H_

class CPF_IBuff
{
public:
    CPF_IBuff();
    ~CPF_IBuff();

    char *m_pbuff;				// I/O操作使用的缓冲区
    int  m_nLen;				    // buff缓冲区（使用的）大小

};

#endif //__CPF_IBuff_H_