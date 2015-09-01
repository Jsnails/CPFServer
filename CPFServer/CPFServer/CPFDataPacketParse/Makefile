# *****************************
# 预定义
# *****************************
CC = g++
AR = ar
RANLIB = ranlib
CFLAGS = -g -O2 -DOTL_ODBC_UNIX
# *****************************
# 变换规则
# *****************************
# Here are some rules for converting .cpp -> .o
.SUFFIXES: .cpp .o
.cpp.o:
	@$(CC) -fPIC $(CFLAGS) ${INCLUDES} -c -g $*.cpp 

# Here are some rules for converting .c -> .o
.SUFFIXES: .c .o
.c.o:
	@$(CC) $(CFLAGS) -c $*.c 
	@echo '----- '$*.c' is compiled ok!'
# 默认超作
default:all

# 主应用文件列表
LIB_BASE_OBJS = CPF_DataPacketParse.o CPF_ParseDataPacketBase.o
LIB_BASE = libCPF_DataPacketParse.so

all: libCPF_DataPacketParse.so clear
			
# 主应用程序
all:$(LIB_BASE) Makefile

$(LIB_BASE): $(LIB_BASE_OBJS)
	$(CC) -shared -o $(LIB_BASE) $(LIB_BASE_OBJS)

# 清理
clean:
	rm -rf *.o  $(LIB_BASE) $(LIB_BASE_OBJS)
clear:	
	mv libCPF_DataPacketParse.so ../lib/
	rm *.o
cl:	
	rm -rf *.o 
