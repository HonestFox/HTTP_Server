#include "sql_api.h"
#include <stdio.h> 

//sql_api::sql_api( const std::string &host = "127.0.0.1", const std::string &user = "root", const std::string &passwd = "", const std::string &db = "database1" )
sql_api::sql_api()
	:_host("127.0.0.1")
	,_user("root")
	 ,_passwd("")
	 ,_db("database1")
{
	_port = 3306;
	_res = NULL;
	_conn = mysql_init(NULL);
}

sql_api::~sql_api()
{}

int sql_api::my_connect_mysql()
{
	int ret = -1;
	std::cout << _conn << " ";
	std::cout << _host.c_str() << " ";
	std::cout << _user.c_str() << " ";
	std::cout << _passwd.c_str() << " ";
	std::cout << _db.c_str() << " ";
	std::cout << _port << " ";
	std::cout << std::endl;
	if( mysql_real_connect( _conn, _host.c_str(), _user.c_str(), _passwd.c_str(), _db.c_str(), _port, NULL, 0 ) == NULL )
	{
		std::cerr << "connect failed" << std::endl;
	}
	else
	{
		ret = 0;
		std::cout << "connect success" << std::endl;
	}
	return ret;
}

int sql_api::my_insert( const std::string &cols, const std::string &data )		//const ??
{
	std::string sql = "INSERT INTO students";
	sql += cols;
	sql += " values ";
	sql += data;

	if( mysql_query(_conn, sql.c_str()) == 0 )
	{
		std::cout << "insert succsee" << std::endl;
	}
	else
	{
		std::cerr << "insert failed" << std::endl;
	}
}

int sql_api::my_select()
{
	std::string sql = "SELECT * FROM students";

	if( mysql_query(_conn, sql.c_str()) == 0 )
	{
		std::cout << "select succsee" << std::endl;
	}
	else
	{
		std::cerr << "select failed" << std::endl;
	}

	_res = mysql_store_result( _conn );
	if( _res )  //!=NULL
	{
		int lines = mysql_num_rows( _res );
		int cols = mysql_num_fields( _res );
		std::cout << "line: " << lines << " cols: " << cols << std::endl;
		//Get Line Name
		int flag = 0;	
		for( MYSQL_FIELD *fn; fn = mysql_fetch_field(_res); ) //! q ! q ! q # # # $# #  
		{
			if( flag == 0 )
			{
				std::cout << fn->name << "\t\t" << "    |   ";
				flag = 1;
			}
			else
			{
				std::cout << fn->name << "\t" << "    |   ";
			}
		}
		std::cout << std::endl;

		flag = 0;
		int i = 0;
		for( ; i < lines; ++i )
		{
			MYSQL_ROW row = mysql_fetch_row(_res);
			int j = 0;
			for( ; j < cols; ++j )
			{
				if( flag == 0 )
				{
					std::cout << row[j] << "\t\t" << "    |   ";
					flag = 1;
				}
				else
				{
					std::cout << row[j] << "\t" << "    |   ";
				}
		//	std::cout << std::endl;
		}
		std::cout << std::endl;
		}
	}
}

int sql_api::my_delete( const std::string &name )
{
	std::string sql = "DELETE FROM students WHERE name='";
	sql+=name;
	sql+="'";
	std::cout << "delete: " << sql << std::endl;

	if( mysql_query(_conn, sql.c_str()) == 0 )
	{
		std::cout << "delete succsee" << std::endl;
	}
	else
	{
		std::cerr << "delete failed" << std::endl;
	}
}

int sql_api::my_update( const std::string &name, const std::string &tag, const std::string &val )
{
	std::cout << "SSSSSSSSSSSSSSSSSSSSSSSSS" << std::endl;
	std::string sql = "UPDATE students SET ";
	sql+=tag;
	sql+="='";
	sql+=val;
	sql+="'";
	sql+=" WHERE name='";
	sql+=name;
	sql+="'";

	std::cout << "update: " << sql << std::endl;

	if( mysql_query(_conn, sql.c_str()) == 0 )
	{
		std::cout << "update succsee" << std::endl;
	}
	else
	{
		std::cerr << "update failed" << std::endl;
	}
}


int sql_api::my_update( const std::string &name, const std::string &tag, const int &val )
{
	std::string sql = "UPDATE students SET ";
	sql+=tag;
	sql+="=";
	char age[10];
	sprintf(age, "%d", val);
	sql+=age;
	sql+=" WHERE name='";
	sql+=name;
	sql+="'";

	std::cout << "update: " << sql << std::endl;

	if( mysql_query(_conn, sql.c_str()) == 0 )
	{
		std::cout << "update succsee" << std::endl;
	}
	else
	{
		std::cerr << "update failed" << std::endl;
	}
}

//int main()
//{
//	sql_api *sql = new sql_api;
//	sql->my_connect_mysql();
//
//	std::cout << mysql_get_client_info() << std::endl;
//	std::string cols = "(name, age, school, register_time)";
//	std::string data = "(\"pass3\", 33, \"SUST\", NOW())";
//
//	sql->my_select();
//	sql->my_insert(cols, data);
//	sql->my_select();
//	sql->my_update("pass3", "school", "BIT");
//	sql->my_select();
//	sql->my_delete("pass3");
//	sql->my_select();
//	return 0;
//}
