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
	printf("exe_cgi\n");
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

	printf("get a new client %d\n  %s\n", sock, path);

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

void clear_header( int sock )
{
	char buf[SIZE];
	int ret = -1;
	int len = SIZE;

	do
	{
		ret = get_line(sock, buf, len);
		fflush(stdout);
	}while(ret > 0 && (strcmp(buf, "\n") != 0) );
}

static void exe_cgi( int sock, const char *method, const char *path, const char *query_string )
{
	char buf[SIZE];
	int content_length = -1;

	//pipe named by child side
	int cgi_input[2];
	int cgi_output[2];
	char method_env[SIZE];
	char query_string_env[SIZE];
	char content_length_env[SIZE];
	char tmpc;
	
	memset(buf, '\0', sizeof(buf));				// ! ! ! ! !  M U S T     I N I T   A R R A Y ! ! !     
	memset(method_env, '\0', sizeof(method_env));
	memset(query_string_env, '\0', sizeof(query_string));
	memset(content_length_env, '\0', sizeof(content_length_env));



	if(strcasecmp(method, "GET") == 0 )
	{
		clear_header(sock);			//"GET" send val by url ,already get
	}

	int ret = 0;
	if(strcasecmp(method, "POST") == 0 )  //different
	{
		do
		{
			memset(buf, '\0', sizeof(buf));
			ret = get_line(sock, buf, sizeof(buf));
			if( strncasecmp(buf, "Content-Length: ", 16) == 0)
			{
				content_length = atoi(&buf[16]);
			}
		}while( (ret > 0) && strcmp(buf, "\n") != 0 );

		printf("content_length = %d\n", content_length);

		if( content_length == -1 )
		{
			echo_errno(sock);  
			return;
		}
	}
/////////////  (8.11)  /////////////////////////////
	
	sprintf(buf, "HTTP/1.0 200 OK\r\n\r\n");
	send(sock, buf, strlen(buf), 0);

	if( pipe(cgi_input) < 0 )
	{
		echo_errno(sock);
		return;
	}
	if( pipe(cgi_output) < 0 )
	{
		echo_errno(sock);
		return;
	}

	pid_t id = fork();
	if(id == 0)		//child
	{
		close(cgi_input[1]);
		close(cgi_output[0]);
  
		dup2(cgi_input[0], 0);
		dup2(cgi_output[1], 1);


		sprintf(method_env, "REQUEST_METHOD=%s", method);
		putenv(method_env);
		if( strcasecmp(method, "GET") == 0 )		//GET	
		{
			sprintf(query_string_env, "QUERY_STRING=%s", query_string);
			putenv(query_string_env);
		}
		else  //POST
		{
			sprintf(content_length_env, "CONTENT_LENGTH=%d", content_length);
			putenv(content_length_env);
		}
		execl(path, path, NULL);
		//run here , execl wrong
		exit(1);
	}
	else  //father
	{
		close(cgi_input[0]);
		close(cgi_output[1]);
		
		dup2(cgi_input[1], 1);
		dup2(cgi_output[0], 0);

		char c = '\0';
		int  i =0;

		if( strcasecmp(method, "POST") == 0 )
		{
			for(; i < content_length; ++i)  
			{
				recv(sock, &c, 1, 0);
				write(cgi_input[1], &c, 1);
			}
		}
		
		int ret = 0;
		while( read(cgi_output[0], &c, 1) > 0)
		{
			printf("%c ", c);
			send(sock, &c, 1, 0);
		}

		waitpid(id, NULL, 0);
	}

}

static void* accept_request(void *arg)		//static
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
//	do
//	{
//		ret = get_line(sock, buf, len);
//		printf("%s", buf);
//		fflush(stdout);
//	}while(ret > 0 && (strcmp(buf, "\n") != 0) );
#endif

	memset(buf, '\0', sizeof(buf));
	memset(method, '\0', sizeof(method));
	memset(url, '\0', sizeof(url));
	memset(path, '\0', sizeof(buf));

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
		sprintf(path, "%s", url+1);
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

	if( strcasecmp(method, "GET") == 0 )
	{
		sprintf(path, "htdoc%s", url);
		if( path[strlen(path)-1] == '/' )
		{
			strcat(path, "index.html");
		}
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
