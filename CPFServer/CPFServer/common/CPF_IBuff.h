#ifndef __CPF_IBuff_H_
#define __CPF_IBuff_H_

class CPF_IBuff
{
public:
    CPF_IBuff();
    ~CPF_IBuff();

    char *m_pbuff;				// I/O����ʹ�õĻ�����
    int  m_nLen;				    // buff��������ʹ�õģ���С

};

#endif //__CPF_IBuff_H_