/*************************************************************************
	> File Name: mysql_cgi.cpp
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Mon 15 Aug 2016 10:25:56 PM CST
 ************************************************************************/

#include<iostream>
#include<string>
#include<cstring>
#include<cstdlib>
#include<stdio.h>
#include "sql_api.h"
//name=zhangsan&age=21&school=sust
void my_insert( char *data[] )
{
	sql_api *sql = new sql_api();
	sql->my_connect_mysql();
	std::string cols = "(name, age, school, register_time)";
	std::string datas = "('";
	datas += data[0];
	datas += "', ";
	datas += data[1];
	datas += ", ";
	datas += " '";
	datas += data[2];
	datas += "', NOW())";
	std::cout << cols << std::endl;
	std::cout << datas << std::endl;

	sql->my_insert( cols, datas );

}

void my_select()
{
	sql_api *sql = new sql_api();
	sql->my_connect_mysql();
	sql->my_select();
}

void my_update( char *data[] )
{
	sql_api *sql = new sql_api();
	sql->my_connect_mysql();
	std::string name = data[0];
	int age = atoi(data[1]);
	
	std::string school = data[2];


	std::cout << age << std::endl;
	sql->my_update(name, "age", age);
	sql->my_update(name, "school", school);
	

}

void my_delete( char *data[] )
{
	sql_api *sql = new sql_api();
	sql->my_connect_mysql();
	sql->my_delete( data[0] );
}

void get_data( char *arg )
{
	char *data[4];
	data[3] = NULL;
	char *end = arg + strlen(arg) - 1;
	int i = 3;

	while( end > arg )
	{
		if( *end == '=' )
		{
			std::cout << "i = " << i << std::endl;
			data[i--] = end+1;
		}
		if( *end == '&' )
		{
			*end = '\0';
		}
		--end;
	}
	

	for( int idx = 0; idx < 4; ++idx )
	{
		std::cout << data[idx] << " ";
	}
	std::cout << std::endl;

	if( strcasecmp( data[3], "insert" ) == 0 )
	{
		my_insert( data );		
	}
	if( strcasecmp( data[3], "select" ) == 0 )
	{
		my_select();		
	}
	if( strcasecmp( data[3], "update" ) == 0 )
	{
		my_update( data );	
	}
	if( strcasecmp( data[3], "delete" ) == 0 )
	{
		my_delete( data );		
	}

}

int main()
{

	char method[1024];
	char arg[1024];
	char content_len[1024];
	int len = -1;
	
	std::cout << "11111111111111111" << std::endl;
	
	
	memset(method, '\0', sizeof(method));
	memset(arg, '\0', sizeof(arg));
	memset(content_len, '\0', sizeof(content_len));
	
	
	
	
	if(getenv("REQUEST_METHOD"))
	{
		strcpy(method, getenv("REQUEST_METHOD"));
	}
	
	if(strcasecmp(method, "GET") == 0)
	{
		if( getenv("QUERY_STRING") )
		{
			strcpy(arg, getenv("QUERY_STRING"));
		}
	}
	else //POST
	{
		if(getenv("CONTENT_LENGTH"))
		{
			strcpy(content_len, getenv("CONTENT_LENGTH"));
		}
		len = atoi(content_len);
	}
	
	int i = 0;
	for(; i < len; ++i)
	{
		read(0, &arg[i], 1);
	}
	arg[i] = '\0';
	
	std::cout << " 22222222222222" << std::endl;
	
	
	get_data(arg);

	return 0;

}
