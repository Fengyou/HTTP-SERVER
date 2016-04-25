ROOT_PATH=$(shell pwd)/..
INCLUDE=-I$(ROOT_PATH)/mysql_connect -I$(ROOT_PATH)/lib/include
LIB_PATH=-L$(ROOT_PATH)/lib/lib
LDFLAGS=-lmysqlclient

REGIST_CGI_SRC=regist_cgi.cpp $(ROOT_PATH)/mysql_connect/mysql_connect.cpp
.PHONY:all
all:math_cgi regist_cgi
math_cgi:math_cgi.cpp
	g++ -o $@ $^
regist_cgi:$(REGIST_CGI_SRC)
	g++ -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LDFLAGS)
.PHONY:clean
clean:
	rm -rf math_cgi regist_cgi
.PHONY:output
output:
	cp math_cgi ../output/htdocs/cgi_bin/
	cp regist_cgi ../output/htdocs/cgi_bin/
