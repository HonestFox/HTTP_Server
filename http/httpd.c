/*************************************************************************
	> File Name: httpd.c
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Tue 09 Aug 2016 04:35:48 PM CST
 ************************************************************************/

#include<stdio.h>
#include<pthread.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/sendfile.h>
#include<fcntl.h>


#define SIZE 1024

void usage(const char *proc)
{
	printf("Usage: %s [ip][port]\n");
}

int get_line(int sock, char buf[], int buflen)
{
	if(!buf || buflen < 0)
	{
		return -1;
	}

	int i = 0;
	char c = '\0';
	int ret = 0;
	while( (i < buflen - 1) && c != '\n' )
	{
		ret = recv(sock, &c, 1, 0);
		if(ret > 0)
		{
			if( c == '\r' )
			{
				ret = recv(sock, &c, 1, MSG_PEEK);
				if(ret > 0 && c == '\n')
				{
					recv(sock, &c, 1, 0);
				}
				else 
				{
					c = '\n';
				}
			}
			else
			{
				buf[i++] = c;
			}
		}
		else
		{
			c = '\n';
		}
	}
	buf[i] = '\0';
	return i;
}

void echo_errno(int sock)
{
	;
}

static void echo_www(int sock, const char *path, ssize_t size)
{
	int fd = open(path, O_RDONLY);  //index.html  and .......	//fcntl
	if(fd < 0)
	{
		echo_errno(sock);
		return;
	}

	char status_line[SIZE];
	sprintf(status_line, "HTTP/1.0 200 OK\r\n\r\n");
	send(sock, status_line, strlen(status_line), 0);


	if( sendfile(sock, fd, NULL, size) < 0 )
	{
		echo_errno(sock);
		return;
	}


	close(fd);
}

static void exe_cgi( int sock, const char *method, const char *path, const char *query_string )
{
	char buf[SIZE];
	int content_length = -1;

	if(strcasecmp(method, "GET") == 0 )
	{
		clear_header(sock);
	}

	if(strcasecmp(method, "POST") == 0 )
	{
		do
		{
			ret = get_line(sock, buf, sizeof(buf));
			if( strncmp(buf, "Content-Length: "), 16 )
			{
				content_length = atoi(&buf[16]);
			}
		}while( (ret > 0) && (strcmp(buf, "\n") != 0) );

		if( content_length == -1 )
		{
			echo_errno(sock);  
			return;
		}
	}
	
	




}

void clear_header( int sock )
{
	char buf[SIZE];
	int ret = -1;
	int len = SIZE;
	do
	{
		ret = get_line(sock, buf, len);
		printf("%s", buf);
		fflush(stdout);
	}while(ret > 0 && (strcmp(buf, "\n") != 0) );
}

static void* accept_request(void *arg)
{
	int sock = (int)arg;
	char buf[SIZE];
	int len = sizeof(buf)/sizeof(*buf);
	char method[SIZE/10];
	char url[SIZE];
	char path[SIZE];
	int ret = -1;
	int i = 0;
	int j = 0;

	char *query_string = NULL;    //aim at val(can shu)
	int cgi = 0;
#ifdef _DEBUG_
	do
	{
		ret = get_line(sock, buf, len);
		printf("%s", buf);
		fflush(stdout);
	}while(ret > 0 && (strcmp(buf, "\n") != 0) );
#endif
	
	//Request Line
	ret = get_line(sock, buf, len);
	if(ret <= 0)
	{
		echo_errno(sock);
		return (void*)1;
	}
	i = 0;	//method index
	j = 0;	//buf index
	while((i < (sizeof(method)/sizeof(*method) - 1)) && (j < sizeof(buf)/sizeof(*buf) - 1) && (!isspace(buf[j]) ) )
	{
		method[i] = buf[j];
		++i;
		++j;
	}
	method[i] = '\0';
	if((strcasecmp(method, "GET") != 0) && (strcasecmp(method, "POST")) )
	{
		//echo_errno(sock);
		return (void*)2;
	}

	//URL
	while(isspace(buf[j]))	//Remove Space
	{
		++j;
	}
	i = 0;
	while( ( i < sizeof(url)/sizeof(*url)-1 ) && (j < sizeof(buf)/sizeof(*buf)) && (!isspace(buf[j])) )
	{
		url[i] = buf[j];
		++i;
		++j;
	}
	url[i] = '\0';


	if( strcasecmp(method, "POST") == 0 )
	{
		cgi = 1;
	}


	if( strcasecmp(method, "GET") == 0 )
	{
		query_string = url;
		while( *query_string != '\0' && *query_string != '?' )
		{
			query_string++;
		}
		if(*query_string == '?')
		{
			cgi = 1;
			*query_string = '\0';
			++query_string;
		}
	}
	
	sprintf(path, "htdoc%s", url);
	if( path[strlen(path)-1] == '/' )
	{
		strcat(path, "index.html");
	}

	struct stat st;
	if( stat(path, &st) < 0 )
	{
		//echo_errno();	//404	
		return (void*)3;
	}
	else	//case : DIR
	{
		if( S_ISDIR(st.st_mode) )
		{
			strcat(path, "index.html");
		}
		else if( (st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH) )      //bin(binary file)
		{
			cgi = 1;
		}
		else
		{}   //noting to do
	}

	printf("method:%s\n", method);
	printf("url:%s\n", url);
	printf("path:%s\n", path);



	if(cgi) // u+x file
	{
		exe_cgi(sock, method, path, query_string);  //cgi mode
	}
	else  //.jpg   .html ...
	{
		clear_header(sock);  //! important!
		echo_www(sock, path, st.st_size);
	}

	close(sock);
	return 0;

	
}


static int startup(const char *_ip, int _port)		//static ?? 
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("socket");
		exit(2);
	}
	
	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(_port);
	local.sin_addr.s_addr = inet_addr(_ip);

	if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
	{
		perror("bind");
		exit(3);
	}

	if(listen(sock, 5) < 0)
	{
		perror("listen");
		exit(4);
	}

	return sock;
}

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		usage(argv[0]);
		exit(1);
	}

	int listen_sock = startup(argv[1], atoi(argv[2]));
	struct sockaddr_in peer;
	socklen_t len = sizeof(peer);

	int done = 0;
	while(!done)
	{
		int new_sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
		if(new_sock > 0)
		{
			pthread_t id;
			pthread_create(&id, NULL, accept_request, (void*)new_sock);
			pthread_detach(id);
		}
	}
	return 0;
}
