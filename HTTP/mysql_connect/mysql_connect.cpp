#include "mysql_connect.h"


sql_connect::sql_connect(const string &host,\
						const string &user,\
						const string &passwd,\
						const string &db)
	:_host(host)
	 ,_user(user)
	 ,_passwd(passwd)
	 ,_db(db)
{
	_mysql_base=mysql_init(NULL);
	_res=NULL;
}

sql_connect::~sql_connect()
{
	close_mysql();
	cout<<"connect close"<<endl;
}

bool sql_connect::connect_mysql()//连接数据库
{
	cout<<"connect success"<<endl;
	if(!mysql_real_connect(_mysql_base,_host.c_str(),_user.c_str(),_passwd.c_str(),_db.c_str(),3306,NULL,0))
	{
		cerr<<"connect error"<<endl;
		return false;
	}
}
bool sql_connect::close_mysql()//关闭数据库
{
	mysql_close(_mysql_base);
}
//插入数据（增）
bool sql_connect::insert_info(const string &info)//在数据库中插入数据
{
	string sql_cmd="INSERT into base_info (name,sex,school,hobby) values ";
	sql_cmd+='(';
	sql_cmd+=info;
	sql_cmd+=')';
	if(mysql_query(_mysql_base,sql_cmd.c_str())==0)
	{
		cout<<"insert success"<<endl;
		return true;
	}
	else
	{
		cout<<"insert error"<<endl;
		return false;
	}
}

//查看数据（查）
bool sql_connect::select_info()
{
	string sql_cmd="select *from base_info";
	if(mysql_query(_mysql_base,sql_cmd.c_str())!=0) //这个函数将读取的信息存储在_mysql_base结构体句柄中
	{
		cout<<"select error"<<endl;
		return false;
	}
	_res=mysql_store_result(_mysql_base);//将句柄中的信息拿到_res中
	if(_res)
	{
		int lines=mysql_num_rows(_res);
		int cols=mysql_num_fields(_res);
		cout<<"lines:"<<lines<<endl;
		cout<<"cols:"<<cols<<endl;

		string *cols_name=new string[cols];
		MYSQL_FIELD *fd=NULL;
		int i=0;
		for(;i<cols;i++)
		{
			fd=mysql_fetch_field(_res);
			cols_name[i]=fd->name;
			cout<<cols_name[i];
			if(i!=cols-1)
			{
				cout<<"   ";
			}
		}
		cout<<endl;
		for(i=0;i<lines;i++)
		{
			MYSQL_ROW row=mysql_fetch_row(_res);//获取一行数据，存储在row中
			for(int j=0;j<cols;j++)
			{
				cout<<row[j];
				if(j!=cols-1)
				{
					cout<<"   ";
				}
			}
			cout<<endl;
		}
		return true;
	}
	return false;
}
//删除数据(删)
bool sql_connect::delete_info(const string &data)
{
	string sql_cmd="select *from base_info";
	if(mysql_query(_mysql_base,sql_cmd.c_str())!=0) //这个函数将读取的信息存储在_mysql_base结构体句柄中
	{
		cout<<"select error"<<endl;
		return false;
	}	
	sql_cmd="delete from base_info where name=";
	sql_cmd+="'";
	sql_cmd+=data;
	sql_cmd+="'";
	
	_res=mysql_store_result(_mysql_base);//将句柄中的信息拿到_res中
	if(_res)
	{
		int lines=mysql_num_rows(_res);
		while(lines>0)
		{
			int i=0;
			MYSQL_ROW row=mysql_fetch_row(_res);//获取一行数据，存储在row中
			if(strcmp(row[i],data.c_str())==0)
			{
				mysql_query(_mysql_base,sql_cmd.c_str());
				cout<<"delete sucess!!!"<<endl;
				return true;
			}
			lines--;
		}
		cout<<"not found,delete error!!!"<<endl;
		return false;
	}
}
//bool sql_connect::modify_info()
//{

//}
#ifdef DEBUG
int main()
{
	sql_connect sql;
	sql.connect_mysql();
	//string data="'yubo','man','qinghua','learn'";
	//sql.insert_info(data);
//	sql.select_info();
	string data="yuantian";
	sql.delete_info(data);
	cout<<"base info:"<<mysql_get_client_info()<<endl;
	return 0;
}
#endif
