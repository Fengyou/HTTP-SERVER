#include "httpd.h"
#define _BACK_LOG_ 10
static void usage(const char *proc)
{
	printf("usage: \033[31m%s [port]\033[0m\n",proc);
}
static void bad_request(int client)
{
	char buf[1024];
	sprintf(buf,"HTTP/1.0 400 BAD REQUEST\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"Content-type: text/html\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"\r\n");
	send(client,buf,strlen(buf),0);

	sprintf(buf,"<html><title>BAD REQUEST</title>\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"<body><p>file is error</p><body>\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"</html>\r\n");
	send(client,buf,strlen(buf),0);
}
static void not_find(int client)
{
	char buf[1024];
	sprintf(buf,"HTTP/1.0 404 NOT FIND\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"Content-type: text/html\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"\r\n");
	send(client,buf,strlen(buf),0);

	sprintf(buf,"<html><title>NOT FIND</title>\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"<body><p>file is error</p><body>\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"</html>\r\n");
	send(client,buf,strlen(buf),0);
}
void return_error_client()
{}
//void return_error_client(int client,int error_code)
//{
//	switch(error_code)
//	{
//		case 400:
//			bad_request(client);
//			break;
//		case 404:
//			//not_found(client);
//			break;
//		case 500:
//			//server_error(client);
//			break;
//		case 503:
//			//server_unavailable(client);
//			break;
//		default:
//			//default_error(client);
//			break;
//	}
//}
int get_line(int client,char buf[],int size)
{
	if(!buf||size<0)
	{
		return -1;
	}
	int i=0;
	char c='\0';
	int n=0;
	while((i<size-1) && (c!='\n'))
	{
		n=recv(client,&c,1,0);//0是阻塞方式
		if(n>0)
		{
			if(c=='\r')
			{
				n=recv(client,&c,1,MSG_PEEK);//只是查探下并不读取
				if(n>0&&c=='\n')//\r\n
				{
					recv(client,&c,1,0);
				}
				else//\r
				{
					c='\n';
				}
			}
			buf[i++]=c;
		}
		else
		{
			c='\n';
		}
	}
	buf[i]='\0';
	return i;
}
int startup(int port)
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("socket");
		exit(2);
	}
	int flag=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(flag));
	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(port);
	local.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		perror("bind");
		exit(3);
	}
	if(listen(sock,_BACK_LOG_)<0)
	{
		perror("listen");
		exit(4);
	}
	return sock;
}

void clear_header(int client)
{
	char buf[2048];
	int ret=0;
	do
	{
		ret=get_line(client,buf,sizeof(buf)-1);
	}while(ret>0 && strcmp(buf,"\n")!=0);
}


void echo_html(int client,char* path,size_t size)
{
	clear_header(client);//将缓冲区的内容清空，包含请求行，消息报头
	int fd=open(path,O_RDONLY,0);
	if(fd<0)
	{
		return_error_client();
		return;
	}
	else
	{
		char buf[1024];
		memset(buf,'\0',sizeof(buf));
		sprintf(buf,"HTTP/1.0 200 OK\r\n\r\n");
		send(client,buf,strlen(buf),0);
		sendfile(client,fd,NULL,size);
	}
	close(fd);
}

void exe_cgi(int client,const char*path,const char*method,const char*query_string)
{
	int num=0;
	char buf[1024];
	int content_length=-1;

	int cgi_input[2]={0};
	int cgi_output[2]={0};

	pid_t pid;

	if(strcasecmp(method,"GET")==0)//如果是get方法，信息都存储在query_string中，因此直接清空client
	{
		clear_header(client);
	}
	else//否则就是POST方法，需要cgi执行
	{
		do
		{
			memset(buf,'\0',sizeof(buf));
			num=get_line(client,buf,sizeof(buf));
			if(strncasecmp(buf,"Content-Length:",15)==0)
			{
				content_length=atoi(&buf[16]);//在消息报头中得到消息正文的长度		
			}
		}while(num>0 && strcmp(buf,"\n")!=0);
		if(content_length==-1)//content_length如果还是-1，则说明请求格式有问题
		{
			return_error_client();
			return;
		}
	}
	sprintf(buf,"HTTP/1.0 200 OK\r\n\r\n");//该请求被受理返回状态行信息
	send(client,buf,strlen(buf),0);

	if(pipe(cgi_input)<0)
	{
		return_error_client();
		return;
	}
	//创建两个管道
	if(pipe(cgi_output)<0)
	{
		close(cgi_input[0]);
		close(cgi_input[1]);
		return_error_client();
		return;
	}

	if((pid=fork())<0)
	{
		close(cgi_input[0]);
		close(cgi_input[1]);
		close(cgi_output[0]);
		close(cgi_output[1]);
		return_error_client();
		return;
	}
	else if(pid==0)//子进程
	{
		//设置三个数组存放环境变量
		char meth_env[1024];
		char query_env[1024];
		char content_len_env[1024];
		memset(meth_env,'\0',sizeof(meth_env));
		memset(query_env,'\0',sizeof(query_env));
		memset(content_len_env,'\0',sizeof(content_len_env));

		close(cgi_input[1]);//cgi脚本从cgi_input读取数据
		close(cgi_output[0]);//cgi脚本从cgi_output输出数据

		dup2(cgi_output[1],1);//cgi脚本的标准输出变成cgi_output管道
		dup2(cgi_input[0],0);//cgi脚本的标准输入变成cgi_input
		sprintf(meth_env,"REQUEST_METHOD=%s",method);
		putenv(meth_env);//设置环境变量

		if(strcasecmp(method,"GET")==0)//GET方法进入cgi，说明client肯定有参数传入
		{
			sprintf(query_env,"QUERY_STRING=%s",query_string);
			putenv(query_env);//将参数导出为环境变量，供cgi脚本使用
		}
		else//POST方法
		{
			sprintf(content_len_env,"CONTENT_LENGTH=%d",content_length);
			putenv(content_len_env);
		}
		execl(path,path,NULL);
		exit(1);
	}
	else//父进程
	{
		int i=0;
		char c;
		close(cgi_input[0]);
		close(cgi_output[1]);
		if(strcasecmp(method,"POST")==0)
		{
			for(;i<content_length;i++)
			{
				recv(client,&c,1,0);
				write(cgi_input[1],&c,1);
			}
		}
		while(read(cgi_output[0],&c,1)>0)
		{
			send(client,&c,1,0);
		}
		close(cgi_input[0]);
		close(cgi_output[1]);
		waitpid(pid,NULL,0);
	}

}
void *accept_request(void *arg)
{
	printf("debug:get a connect\n");
	pthread_detach(pthread_self());
	int client=(int)arg;
	char path[1024];
	int cgi=0;
	char*query_string=NULL;//指向url的指针
	char buf[1024];
	char method[100];
	char url[2048];
	memset(method,'\0',sizeof(method));
	memset(url,'\0',sizeof(url));
	memset(buf,'\0',sizeof(buf));
	memset(path,'\0',sizeof(path));
//#ifdef DEBUG
//	while(get_line(client,buf,sizeof(buf))>0)
//	{
//		printf("%s");
//	}
//#endif

	if(get_line(client,buf,sizeof(buf)-1)<0)//得到了一个链接，但是读取一行失败
	{
		return_error_client();
		close(client);
		return (void *)-1;
	}
	printf("debug:%s\n",buf);
	//读取成功，得到了请求行存储在buf中
	//请求行格式是 GET / HTTP/1.1
	int i=0;//buf下标
	int j=0;//method下标
	while(!isspace(buf[i]) && i<sizeof(buf)-1 && j<sizeof(method)-1)//提取到方法
	{
		method[j]=buf[i];
		i++;
		j++;
	}
	if(strcasecmp(method,"GET") && strcasecmp(method,"POST"))//判断是不是GET或POST方法
	{
		return_error_client();
		close(client);
		return (void *)-1;
	}
	if(!strcasecmp(method,"POST"))
	{
		cgi=1;
	}
	while(isspace(buf[i])&&i<sizeof(buf))//跳过空格
	{
		i++;
	}
	j=0;//此时j为url的下标
	while(!isspace(buf[i]) && j<sizeof(url)-1 && i<sizeof(buf))//将buf中存储的url信息放在url数组中
	{
		url[j]=buf[i];
		i++;
		j++;
	}
	if(strcasecmp(method,"GET")==0)//判断在GET方法下是不是传递了参数
	{
		query_string=url;
		while(*query_string!='?' && *query_string!='\0')
		{
			query_string++;
		}
		if(*query_string=='?')//遇到了？说明传递了参数，必须要通过cgi来处理
		{
			*query_string='\0';		
			query_string++;
			cgi=1;
		}
	}
	sprintf(path,"htdocs%s",url);//将路径存储在path中	
	if(path[strlen(path)-1]=='/')//处理根目录路径的情况
	{
		strcat(path,MAIN_PAGE);//将index.html追加到后面
	}
	
	struct stat st;
	int ret=stat(path,&st);
	if(ret<0)//所要请求路径资源不存在
	{
		return_error_client();
		close(client);
		return (void*)-1;
	}
	else//存在
	{
		if(S_ISDIR(st.st_mode))//请求的路径是目录
		{
			strcat(path,"/");
			strcat(path,MAIN_PAGE);
		}
		else if((st.st_mode & S_IXUSR)||(st.st_mode & S_IXGRP)||(st.st_mode & S_IXOTH))//具有可执行权限，通过CGI方式执行
		{
		//	printf("path:%s\n",path);
		//	printf("method:%s\n",method);
		//	printf("query_string:%s\n",query_string);
			cgi=1;
		}

		if(cgi)
		{
			exe_cgi(client,path,method,query_string);
		}
		else
		{
			echo_html(client,path,st.st_size);
		}
}
	close(client);
	return (void*)0;
}
int main(int argc,char *argv[])
{
	if(argc!=2)
	{
		usage(argv[0]);
		exit(1);
	}
	int port=atoi(argv[1]);
	int listen_sock=startup(port);
	struct sockaddr_in client;
	socklen_t len=sizeof(client);
	while(1)
	{
		int new_client=accept(listen_sock,(struct sockaddr*)&client,&len);
		if(new_client<0)
		{
			perror("accept");
			continue;
		}
		pthread_t id;
		pthread_create(&id,NULL,accept_request,(void*)new_client);
	}
	close(listen_sock);
	return 0;
}
