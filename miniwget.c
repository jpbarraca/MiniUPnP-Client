/* Project : miniupnp
 * Author : Thomas Bernard
 * Copyright (c) 2005 Thomas Bernard
 * This software is subject to the conditions detailed in the
 * LICENCE file provided in this distribution.
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <winsock2.h>
#include <io.h>
#define MAXHOSTNAMELEN 64
#define MIN(x,y) (((x)<(y))?(x):(y))
#define snprintf _snprintf
#define herror
#else
#include <unistd.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#endif

void * miniwget2(const char * url, const char * host,
				 unsigned short port, const char * path,
				 int * size)
{
	char buf[2048];
    int s;
	struct sockaddr_in dest;
	struct hostent *hp;
	*size = 0;
	hp = gethostbyname(host);
	if(hp==NULL)
	{
		herror(host);
		return NULL;
	}
	memcpy((char *)&dest.sin_addr, hp->h_addr, hp->h_length);
	memset(dest.sin_zero, 0, sizeof(dest.sin_zero));
	s = socket(PF_INET, SOCK_STREAM, 0);
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	if(connect(s, (struct sockaddr *)&dest, sizeof(struct sockaddr))<0)
	{
		perror("connect");
		return NULL;
	}

	snprintf(buf, 2048,
                 "GET %s HTTP/1.1\r\n"
			     "Host: %s:%d\r\n"
				 "Connection: Close\r\n"
				 "\r\n",
		    path, host, port);
	write(s, buf, strlen(buf));
	{
		int n, headers=1;
		char * respbuffer = NULL;
		//int contentlength = -1;
		int allreadyread = 0;
		while((n = read(s, buf, 2048)) > 0)
		{
			if(headers)
			{
				int i=0;
				while(i<n-4)
				{
//					fprintf(stderr,"%c",buf[i]);
//					if(buf[i]=='\a' && buf[i+1]=='\n'
//					   && buf[i+2]=='\r' && buf[i+3]=='\n')
					if(buf[i]=='<' && buf[i+1]=='?'
					   && buf[i+2]=='x' && buf[i+3]=='m' && buf[i+4] =='l')
					{
						headers = 0;	// fini !
						if(i<n-4)
						{
							respbuffer = (char *)realloc((void *)respbuffer, 
														 allreadyread+(n-i-4));
							memcpy(respbuffer+allreadyread, buf + i + 4, n-i-4);
							allreadyread += (n-i-4);
						}
						break;
					}
					i++;
				}
			}
			else
			{
				respbuffer = (char *)realloc((void *)respbuffer, 
								 allreadyread+n);
				memcpy(respbuffer+allreadyread, buf, n);
				allreadyread += n;
			}
		}
		if(n<0)
		{
			perror("read");
		}
		*size = allreadyread;
//		printf("%d octets copies (%d)\n", *size, n);
		close(s);
		return respbuffer;
	}
}

//retourne 0 si echec
int parseURL(const char * url, char * hostname, unsigned short * port, char * * path)
{
	char * p1, *p2, *p3;
	p1 = strstr(url, "://");
	if(!p1)
		return 0;
	p1 += 3;
	if(  (url[0]!='h') || (url[1]!='t')
	   ||(url[2]!='t') || (url[3]!='p'))
		return 0;
	p2 = strchr(p1, ':');
	p3 = strchr(p1, '/');
	if(!p3)
		return 0;
	memset(hostname, 0, MAXHOSTNAMELEN + 1);
	if(!p2 || (p2>p3))
	{
		strncpy(hostname, p1, MIN(MAXHOSTNAMELEN, (int)(p3-p1)));
		*port = 80;
	}
	else
	{
		strncpy(hostname, p1, MIN(MAXHOSTNAMELEN, (int)(p2-p1)));
		*port = 0;
		p2++;
		while( (*p2 >= '0') && (*p2 <= '9'))
		{
			*port *= 10;
			*port += (unsigned short)(*p2 - '0');
			p2++;
		}
	}
	*path = p3;
	return 1;
}

void * miniwget(const char * url, int * size)
{
	unsigned short port;
	char * path;
	// protocol://host:port/chemin
	char hostname[MAXHOSTNAMELEN+1];
	*size = 0;
	if(!parseURL(url, hostname, &port, &path))
		return NULL;
//	printf("url : %s\nhost : %s\nport : %d\npath : %s\n",
//	       url, hostname, port, path);

	return miniwget2(url, hostname, port, path, size);
}

