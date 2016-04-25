ROOT_PATH=$(shell pwd)
SERVER_BIN=httpd
CLIENT_BIN=demo_client
BIN_DIR=$(shell pwd)/cgi_bin

CC=gcc
#FLAGS=-DDEBUG
FLAGS=
LDFLAGS=-lpthread

SERVER_SRC=httpd.c
CLIENT_SRC=demo_client.c

.PHONY:all
all:$(SERVER_BIN) $(CLIENT_BIN) cgi

$(SERVER_BIN):$(SERVER_SRC)
	$(CC) -o $@ $^ $(FLAGS) $(LDFLAGS)
$(CLIENT_BIN):$(CLIENT_SRC)
	$(CC) -o $@ $^ $(FLAGS) $(LDFLAGS)

.PHONY:cgi
cgi:
	for name in `echo $(BIN_DIR)`;\
	do\
		cd $$name;\
		make;\
		cd -;\
	done

.PHONY:output
output:
	mkdir output
	cp httpd output
	cp start.sh output
	cp demo_client output
	cp -rf htdocs output
	cp -rf cgi_bin htdocs/
	cp -rf conf output
	cp -rf log output
	mkdir -p output/htdocs/cgi_bin
	for name in `echo $(BIN_DIR)`;\
	do\
		cd $$name;\
		make output;\
		cd -;\
	done
	cp -rf lib output
.PHONY:clean
clean:
	rm -rf $(SERVER_BIN) $(CLIENT_BIN) output
	rm -rf htdocs/cgi_bin
	for name in `echo $(BIN_DIR)`;\
	do\
		cd $$name;\
		make clean;\
		cd -;\
	done
