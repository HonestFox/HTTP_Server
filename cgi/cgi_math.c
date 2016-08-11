/*************************************************************************
	> File Name: cgi_math.c
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Thu 11 Aug 2016 11:23:01 AM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
 

static void mymath(char *arg)
{
	//data1=100&data2=200
	char *end = arg + strlen(arg) - 1;
	char *data[3];
	data[2] = NULL;
	int i = 1;

	while( end > arg )
	{
		if( *end == '=' )
		{
			data[i--] = end + 1;
		}
		if( *end = '&' )
		{
			*end = '\0';
			end--;
		}
	}

	printf("%s+%s=%d\n", data[0], data[1], atoi(data[0])+atoi(data[1]));

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
		printf("method: %d\n", method);
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

	printf("")
	printf("method: %s\n", method);
	printf("arg:%s\n", method);
	printf("len:%d\n", len);

	return 0;
}
