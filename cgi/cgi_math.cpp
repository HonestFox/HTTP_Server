/*************************************************************************
	> File Name: cgi_math.cpp
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Thu 11 Aug 2016 11:23:01 AM CST
 ************************************************************************/

#include<iostream>
#include<string>
#include<cstring>
#include<cstdlib>
#include<cstdio>
#include"BigData.h"

using namespace std;

static void mymath(char *arg)
{
	//data1=100&data2=200
	char *end = arg + strlen(arg) - 1;
	char *data[3];
	data[2] = NULL;
	int i = 2;

	while( end > arg )
	{
		if( *end == '=' )
		{
			data[i--] = end + 1;
		}
		if( *end == '&' )  // == == == == == !!!!!!!!!!!!!!!!!
		{
			*end = '\0';
		}
		--end;
	}

	cout << data[0] << endl;
	cout << data[1] << endl;

	BigData data1(data[0]);
	BigData data2(data[1]);
	char *CalMethod = data[2];
	

	if(strcmp(CalMethod, "%2B") == 0)
	{
		BigData data3 = data1 + data2;
		cout << data1 << " + " << data2 << " = " << data3 << endl;
	}
	if(strcmp(CalMethod, "-") == 0)
	{
		BigData data3 = data1 - data2;
		cout << data1 << " - " << data2 << " = " << data3 << endl;
	}
	if(strcmp(CalMethod, "*") == 0)
	{
		BigData data3 = data1 * data2;
		cout << data1 << " * " << data2 << " = " << data3<< endl;
	}
	if(strcmp(CalMethod, "%2F") == 0)
	{
		BigData data3 = data1 / data2;
		cout << data1 << " / " << data2 << " = " << data3 << endl;
	}

//
//	if(strcmp( data[2], "%2B" ) == 0)
//	{
//		printf("%s+%s=%d\n", data[0], data[1], atoi(data[0])+atoi(data[1]));
//	}
//	if(strcmp( data[2], "-" ) == 0)
//	{
//		printf("%s-%s=%d\n", data[0], data[1], atoi(data[0])-atoi(data[1]));
//	}
//	if(strcmp( data[2], "*" ) == 0)
//	{
//		printf("%s*%s=%d\n", data[0], data[1], atoi(data[0])*atoi(data[1]));
//	}
//	if(strcmp( data[2], "%2F" ) == 0)
//	{
//		float ret = (float)atoi(data[0]) / (float)atoi(data[1]);
//		printf("%s/%s=%f\n", data[0], data[1], ret);
//	}

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
	mymath(arg);
	return 0;
}
