#include <iostream>
#include <error.h>
#include <string>
#include <string.h>
#include "mysql.h"
using namespace std;

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_USER "root"
#define DEFAULT_PASSWD ""
#define DEFAULT_DB "students"
class sql_connect
{
	public:
		sql_connect(const string &host=DEFAULT_IP,\
						const string &user=DEFAULT_USER,\
						const string &passwd=DEFAULT_PASSWD,\
						const string &db=DEFAULT_DB);
		~sql_connect();

		bool connect_mysql();//连接数据库
		bool insert_info(const string &info);
		bool select_info();
		bool delete_info(const string &data);
		bool close_mysql();
	private:
		MYSQL_RES *_res;
		MYSQL *_mysql_base;
		string _host;
		string _user;
		string _passwd;
		string _db;
};

