/* Project : miniupnp
 * Author : Thomas Bernard
 * Copyright (c) 2005 Thomas Bernard
 * This software is subject to the conditions detailed in the
 * LICENCE file provided in this distribution.
 *
 * implementation minimale du soap
 */
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <io.h>
#define snprintf _snprintf
#else
#include <unistd.h>
#endif
#include "minisoap.h"

// only for malloc
#include <stdlib.h>

/* httpWrite ecrit les headers puis le body sur
 * le filedesc et retourne le nombre total d'octets ecrits */
int httpWrite(int fd, const char * body, int bodysize, const char * headers)
{
	int n = 0;
	//n = write(fd, headers, strlen(headers));
	//if(bodysize>0)
	//	n += write(fd, body, bodysize);
	/* Note : my old linksys router only took into account
	 * soap request that are sent into only one packet */
	int i;
	char * p;
	i = strlen(headers);
	p = malloc(i+bodysize);
	memcpy(p, headers, i);
	memcpy(p+i, body, bodysize);
	n = write(fd, p, i+bodysize);
	free(p);
	return n;
}

/* relativement clair */
int soapPostSubmit(int fd,
                   const char * url,
				   const char * host,
				   unsigned short port,
				   const char * action,
				   const char * body)
{
	int bodysize;
	char headerbuf[1024];
	bodysize = strlen(body);
	snprintf(headerbuf, 1024,
                       "POST %s HTTP/1.1\r\n"
	                   "HOST: %s:%d\r\n"
	                   "Content-length: %d\r\n"
					   "Content-Type: text/xml\r\n"
					   "SOAPAction: \"%s\"\r\n"
					   "\r\n",
					   url, host, port, bodysize, action);
	//puts(headerbuf);
	//fwrite(body, 1, bodysize, stdout);
	return httpWrite(fd, body, bodysize, headerbuf);
}


