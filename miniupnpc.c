/* $Id: miniupnpc.c,v 1.10 2006/05/27 10:44:30 nanard Exp $ */
/* Project : miniupnp
 * Author : Thomas BERNARD
 * copyright (c) 2005 Thomas Bernard
 * This software is subjet to the conditions detailed in the
 * provided LICENCE file. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <winsock2.h>
#include <io.h>
#define snprintf _snprintf
#define strncasecmp memicmp
#define MAXHOSTNAMELEN 64
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#endif
#include "miniupnpc.h"
#include "miniwget.h"
#include "minisoap.h"
#include "minixml.h"

/* parsing de la root description */
void parserootdesc(const char * buffer, int bufsize, struct IGDdatas * data)
{
	struct xmlparser parser;
	// objet xmlparser
	parser.xmlstart = buffer;
	parser.xmlsize = bufsize;
	parser.data = data;
	parser.starteltfunc = IGDstartelt;
	parser.endeltfunc = IGDendelt;
	parser.datafunc = IGDdata;
	parsexml(&parser); // goon !
	//printIGD(data);
}

void parseservicedesc(const char* buffer, int bufsize, struct IGDdatas* data, const unsigned devid, const unsigned sid)
{
	struct xmlparser parser;
	// objet xmlparser
	if(!data)
		return;

	if(devid >= data->devices || sid >= data->dlist[devid]->services)
		return;

	data->curdevice = devid;
	data->dlist[devid]->curservice = sid;
	data->state = 0;
	data->level = 0;

	parser.xmlstart = buffer;
	parser.xmlsize = bufsize;
	parser.data = data;
	parser.starteltfunc = IGDstartelt;
	parser.endeltfunc = IGDendelt;
	parser.datafunc = IGDdata;
	parsexml(&parser); // goon !
//	printIGD(data);
}

/* simpleUPnPcommand :
 * pas si simple que ca...
 *  */
int simpleUPnPcommand(int s, const char * url, const char * service,
                      const char * action, struct UPNParg * args,
                      char * buffer, int * bufsize)
{
	struct sockaddr_in dest;
	char hostname[MAXHOSTNAMELEN+1];
	unsigned short port = 0;
	char * path;
	char soapact[128];
	char soapbody[2048];	// ATTENTION !
	char * buf;
	int buffree;
    int n;
	snprintf(soapact, 128, "%s#%s", service, action);
	if(args==NULL)
	{
		snprintf(soapbody, 2048, "<?xml version=\"1.0\"?>\r\n"
	    	              "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
						  "<SOAP-ENV:Body>"
						  "<m:%s xmlns:m=\"%s\"/>"
						  "</SOAP-ENV:Body></SOAP-ENV:Envelope>"
					 	  "\r\n", action, service);
	}
	else
	{
		char * p;
		const char * pe, * pv;
		snprintf(soapbody, 2048, "<?xml version=\"1.0\"?>\r\n"
	    	              "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
						  "<SOAP-ENV:Body>"
						  "<m:%s xmlns:m=\"%s\">",
						  action, service);
		p = soapbody+strlen(soapbody);
		while(args->elt)
		{
			*(p++) = '<';
			pe = args->elt;
			while(*pe)
				*(p++) = *(pe++);
			*(p++) = '>';
			if((pv = args->val))
			{
				while(*pv)
					*(p++) = *(pv++);
			}
			*(p++) = '<';
			*(p++) = '/';
			pe = args->elt;
			while(*pe)
				*(p++) = *(pe++);
			*(p++) = '>';
			args++;
		}
		*(p++) = '<';
		*(p++) = '/';
		*(p++) = 'm';
		*(p++) = ':';
		pe = action;
		while(*pe)
			*(p++) = *(pe++);
		strcpy(p, "></SOAP-ENV:Body></SOAP-ENV:Envelope>\r\n");
	}
	if(!parseURL(url, hostname, &port, &path)) return -1;
	if(s<0)
	{
		s = socket(PF_INET, SOCK_STREAM, 0);
		dest.sin_family = AF_INET;
		dest.sin_port = htons(port);
		dest.sin_addr.s_addr = inet_addr(hostname);
		if(connect(s, (struct sockaddr *)&dest, sizeof(struct sockaddr))<0)
		{
			perror("connect");
			return -1;//exit(1);
		}
	}

	//printf("------- %s\n", action);
	n = soapPostSubmit(s, /*url*/path, hostname, port, soapact, soapbody);
	//printf("====> %d octets ecrits\n", n);

	buf = buffer;
	buffree = *bufsize;
	*bufsize = 0;
	while ((n = read(s, buf, buffree)) > 0) {
	    //buf[n] = '\0';
	    //printf("<========= %d octets lus :\n%s\n----------\n", n, buf);
		buffree -= n;
		buf += n;
		*bufsize += n;
	}
	
	close(s);
	return -1;//s;
}

// parse la reponse au M-SEARCH
// renvoie dans les 2 derniers parametres la chaine trouvée et sa taille
void parseMSEARCHReply(const char * reply, int size,
                       const char * * location, int * locationsize)
{
	int a, b, i;
	i = 0;
	a = i;	// debut de la ligne
	b = 0;
	while(i<size)
	{
		switch(reply[i])
		{
		case ':':
				if(b==0)
				{
					b = i; // fin du "header"
					/*for(j=a; j<b; j++)
					{
						putchar(reply[j]);
					}
					*/
				}
				break;
		case '\x0a':
		case '\x0d':
				if(b!=0)
				{
					/*for(j=b+1; j<i; j++)
					{
						putchar(reply[j]);
					}
					putchar('\n');*/
					if(0==strncasecmp(reply+a, "location", 8))
					{
						b++;
						while(reply[b]==' ') b++;
						*location = reply+b;
						*locationsize = i-b;
					}
					b = 0;
				}
				a = i+1;
				break;
		default:
				break;
		}
		i++;
	}
}

/* port upnp decouverte */
#define PORT (1900)
#define UPNP_MCAST_ADDR ("239.255.255.250")

/* upnpDiscover :
 * renvoie NULL si rien n'a été trouvé, l'url de la root description sinon
 * ne pas oublier d'appeller free() sur le resultat
 * le delay est exprime en milliseconde (poll) */
char * upnpDiscover(int delay)
{
	static char MSearchMsg[] = 
	"M-SEARCH * HTTP/1.1\r\n"
	"HOST: 239.255.255.250:1900\r\n"
	//"ST:urn:schemas-upnp-org:device:InternetGatewayDevice:1\r\n"
	"ST:upnp:rootdevice\r\n"
	"MAN:\"ssdp:discover\"\r\n"
	"MX:3\r\n"
	"\r\n";
	char bufr[2048];	// buffer de reception
	int sudp;
	int n;
	struct sockaddr_in sockudp_r, sockudp_w;
#ifndef WIN32
	// TODO : find a way on win32... => use select() !
	struct pollfd fds[1]; // pour le poll
#endif
	
#ifdef WIN32
	sudp = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
#else
	sudp = socket(PF_INET, SOCK_DGRAM, 0);
#endif
    /* reception */
    memset(&sockudp_r, 0, sizeof(struct sockaddr_in));
    sockudp_r.sin_family = AF_INET;
    sockudp_r.sin_port = htons(PORT);
    sockudp_r.sin_addr.s_addr = INADDR_ANY;
    /* emission */
    memset(&sockudp_w, 0, sizeof(struct sockaddr_in));
    sockudp_w.sin_family = AF_INET;
    sockudp_w.sin_port = htons(PORT);
    sockudp_w.sin_addr.s_addr = inet_addr(UPNP_MCAST_ADDR);

	/* Avant d'envoyer le paquet on bind pour recevoir la reponse */
    if (bind(sudp, (struct sockaddr *)&sockudp_r, sizeof(struct sockaddr_in)) < 0)
	{
        perror("bind");
		close(sudp);
		return NULL;
    }
	/* emmission du paquet SSDP */
	n = sendto(sudp, MSearchMsg, strlen(MSearchMsg), 0, 
	          (struct sockaddr *)&sockudp_w, sizeof(struct sockaddr_in));
    if (n < 0) {
        perror("sendto");
		close(sudp); 
		return NULL;
	}
	/* attente de la reception de la reponse */
#ifndef WIN32
	fds[0].fd = sudp;
	fds[0].events = POLLIN;
	n = poll(fds, 1, delay);
	if(n < 0)
	{
		perror("poll");
		return NULL;
	}
	else if(n == 0)
	{
		fprintf(stderr, "timeout waiting for M-SEARCH response\n");
		return NULL;
	}
#endif
    n = recv(sudp, bufr, sizeof(bufr), 0);
	if (n < 0) {
		perror("recv");
		close(sudp);
		return NULL;
	} else if (n == 0) {
		// pas de donnees...
		close(sudp);
		return NULL;
	} else {
		const char * descURL=NULL;
		char * retString;
		int urlsize=0;
        //printf("%d byte(s) :\n%s\n", n, bufr); /* affichage du message */
		close(sudp);
		parseMSEARCHReply(bufr, n, &descURL, &urlsize);
		if(descURL)
		{
			retString = malloc(urlsize+1);
			memcpy(retString, descURL, urlsize);
			retString[urlsize] = '\0';
			return retString;
		}
		else
			return NULL;
	}
}

/*
 * */
void GetUPNPUrls(struct UPNPUrls * urls, struct IGDdatas * data, const char* servicetype,
                 const char * descURL)
{
	urls->controlURL = malloc(256);
	urls->ipcondescURL = malloc(256);
	int d;
	struct IGDservice * srv = NULL;

	// maintenant on chope la desc du WANIPConnection

	if(data->urlbase[0] != '\0')
		strcpy(urls->ipcondescURL, data->urlbase);
	else
	{
		char * p;	// recupere l'url de la desc.
		strcpy(urls->ipcondescURL, descURL);
		p = strchr(urls->ipcondescURL+7, '/');
		if(p) p[0] = '\0';
	}
	strcpy(urls->controlURL, urls->ipcondescURL);
	//printf("controlURL=%s\n", urls->controlURL);
	//
	char servicetype_full[64] ="urn:schemas-upnp-org:service:";
	strncat(servicetype_full,servicetype,63);

	for(d = 0;d<data->devices;d++)
	{
		struct IGDdevice * dev	= data->dlist[d];
		int s;
		for(s=0;s<dev->services;s++){
			if(!strcmp(dev->slist[s]->servicetype,servicetype_full))
			{
				srv = dev->slist[s];
				break;
			}
		}
		if(srv)
			break;
	}
	if(srv->scpdurl[0] != '/')
		strcat(urls->ipcondescURL, "/");	
	strcat(urls->ipcondescURL, srv->scpdurl);

	if(  (srv->controlurl[0] == 'h')
	   &&(srv->controlurl[1] == 't')				
	   &&(srv->controlurl[2] == 't')				
	   &&(srv->controlurl[3] == 'p')
	   &&(srv->controlurl[4] == ':')
	   &&(srv->controlurl[5] == '/'))
	{
		strcpy(urls->controlURL, srv->controlurl);
	}
	else
	{
		if(srv->controlurl[0] != '/')
			strcat(urls->controlURL, "/");
		strcat(urls->controlURL, srv->controlurl);
	}
//	printf("controlURL=%s\n", controlURL);

}

void
FreeUPNPUrls(struct UPNPUrls * urls)
{
	if(!urls)
		return;
	free(urls->controlURL);
	urls->controlURL = 0;
	free(urls->ipcondescURL);
	urls->ipcondescURL = 0;
}


