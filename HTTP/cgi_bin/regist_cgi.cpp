#include <iostream>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
#include "mysql_connect.h"

using namespace std;
//name=hello&sex=man&school=xatu&hobby=sleeping
void regist_sql(char *query_string)
{
	sql_connect sql;
	int num=0;
	char *tmp=query_string;
	while(*tmp!='\0')
	{
		if(*tmp=='=')
		{
			num++;//统计信息的个数
		}
		tmp++;
	}
	string *arr=new string[num];
	int index=strlen(query_string)-1;
	int i=num-1;
	int count=0;
	while(index>=0)
	{
		if(query_string[index]=='&')
		{
			query_string[index]='\0';
		}
		if(query_string[index]=='=')
		{
			arr[i]=&query_string[index+1];//将提取出来的数据存储在arr中
			count++;
			i--;
			if(i<0)
			{
				break;
			}
		}
		index--;
	}

	string info="";
	if(count==1)
	{
		info=arr[0];
	}
	else
	{
		for(i=0;i<count;i++)
		{
			info+="'";
			info+=arr[i];
			info+="'";
			if(i!=3)
			{
				info+=",";
			}
		}
	}
	sql.connect_mysql();
	sql.insert_info(info);
	//sql.delete_info(info);
	sql.select_info();
	return;
}
//name=hello&sex=man&school=xatu&hobby=sleeping
//void regist_sql(char *query_string)
//{
//	sql_connect sql;
//	string arr[4];
//	int index=strlen(query_string)-1;
//	int i=3;
//	while(index>=0)
//	{
//		if(query_string[index]=='&')
//		{
//			query_string[index]='\0';
//		}
//		if(query_string[index]=='=')
//		{
//			arr[i]=&query_string[index+1];
//			i--;
//		}
//		index--;
//	}
//	string info="";
//	for(i=0;i<4;i++)
//	{
//		info+="'";
//		info+=arr[i];
//		info+="'";
//		if(i!=3)
//		{
//			info+=",";
//		}
//	}
//	sql.connect_mysql();
//	//sql.insert_info(info);
//	sql.delete_info(info);
//	sql.select_info();
//	return;
//}

int main()
{

	string method;
	string query_string;
	string content_length;
	char buf[1024];
	memset(buf,'\0',sizeof(buf));
	if(getenv("REQUEST_METHOD")==NULL)
	{
		exit(1);
	}
	method=getenv("REQUEST_METHOD");
	if(strcasecmp(method.c_str(),"GET")==0)//get方法
	{
		if(getenv("QUERY_STRING")==NULL)
		{
			exit(1);
		}
		query_string=getenv("QUERY_STRING");
		char buf[1024];
		memset(buf,'\0',sizeof(buf));
		strncpy(buf,query_string.c_str(),query_string.size());
		regist_sql(buf);
	}
	else if(strcasecmp(method.c_str(),"POST")==0)
	{
		if(getenv("CONTENT_LENGTH")==NULL)
		{
			exit(1);
		}
		content_length=getenv("CONTENT_LENGTH");
		int len=atoi(content_length.c_str());
		int i=0;
		for(;i<len;i++)
		{
			read(0,&buf[i],1);
		}
		regist_sql(buf);
	}
	else
	{
		exit(1);
	}
	return 0;
}

