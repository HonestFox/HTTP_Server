/*************************************************************************
	> File Name: cgi_select.cpp
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Thu 11 Aug 2016 11:23:01 AM CST
 ************************************************************************/

#include<iostream>
#include<string>
#include<cstring>
#include<cstdlib>
#include<cstdio>

#include "mysql.h"
//name=zhangsan&school=sust&hobby=coding

void myselect( char *arg )
{
	sql_api *sql = new sql_api;

	std::string cols = "(name, school, hobby)";

	char *buf[4];
	buf[3] = NULL;
	int index = 2;

	char *end = arg + strlen(arg) - 1;
	while(end > arg)
	{
		if( *end == '=' )
		{
			buf[index--] = end + 1;
		}
		if( *end == '&' )
		{
			buf[index] = '\0';
		}
		--end;
	}

	std::string data = "(\"";
	data += buf[0];
	data += "\",\"";
	data += buf[1];
	data += "\",\"";
	data += buf[2];
	data += ")";

	sql->my_connect_mysql();
	sql->my_insert(cols, data);

}

void UnitTest()
{
	char arg[] = "name=AAA&school=BBB&hobby=CCC";
	myselect(arg);
}


int main()
{
	char method[1024];
	char arg[1024];
	char content_len[1024];
	int len = -1;

	

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
	myselect(arg);
	return 0;

}
