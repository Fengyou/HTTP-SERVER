ROOT_PATH=$(shell pwd)/..
INCLUDE=-I$(ROOT_PATH)/lib/include
#-I选项让编译器额外的在该路径下找需要的头文件
LIB_PATH=-L$(ROOT_PATH)/lib/lib
#指定库文件的位置
LDFLAGS=-lmysqlclient



mysql_connect:mysql_connect.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LDFLAGS) -DDEBUG
.PHONY:clean
clean:
	rm -rf mysql_connect
