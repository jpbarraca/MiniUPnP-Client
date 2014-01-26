/* Project : miniupnp
 * Author : Thomas Bernard
 * Copyright (c) 2005 Thomas Bernard
 * This software is subject to the conditions detailed in the
 * LICENCE file provided in this distribution.
 * */
#include "igd_desc_parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void IGDstartelt(void * d, const char * name, int l)
{
	struct IGDdatas * datas = (struct IDGdatas *) d;
	memcpy( datas->cureltname, name, l);
	datas->cureltname[l] = '\0';

	if(!strncmp(name,"argument",l) || 
		 !strncmp(name,"action",l) || 
		 !strncmp(name,"service",l) || 
		 !strncmp(name,"device",l))
		 {
				memcpy( datas->curmaineltname, name, l);
				datas->curmaineltname[l] = '\0';
		 }
	datas->level++;
}

void IGDendelt(void * d, const char * name, int l)
{
	struct IGDdatas * datas = (struct IDGdatas *)d;
	datas->level--;

	if(l == 8 && !memcmp(name,"argument",l)){
		struct IGDdevice * dev = datas->dlist[datas->curdevice];
		struct IGDservice * srv = dev->slist[dev->curservice];
		struct IGDaction * act = srv->alist[srv->curaction];
		act->arguments++;
		act->curargument++;
		datas->curmaineltname[0]='\0';
	}
	else
	if(l == 6 && !memcmp(name,"action",l)){
		struct IGDdevice * dev = datas->dlist[datas->curdevice];
		struct IGDservice * srv = dev->slist[dev->curservice];
		srv->actions++;
		srv->curaction++;
		datas->curmaineltname[0]='\0';
	}
	else				
	if(l == 7 && !memcmp(name,"service",l)){
		datas->dlist[datas->curdevice]->services++;
		datas->dlist[datas->curdevice]->curservice++;
		datas->curmaineltname[0]='\0';
	}
	else
	if(l == 6 && !memcmp(name,"device",l)){
		datas->devices++;
		datas->curdevice++;
		datas->curmaineltname[0]='\0';
	}
	else
	if((l == 4 && !memcmp(name,"root",l)) || (l == 5 && !memcmp(name,"scpd",l))){
		datas->state++; //End of the line :)
		datas->curmaineltname[0]='\0';
	}
}

void IGDdata(void * d, const char * data, int l)
{
	struct IGDdatas * datas = (struct IDGdatas *)d;
	char * dstmember = 0;
//	printf("%2d %s : %s\n",
 //         datas->level, datas->cureltname, data);	
	
	if( !strcmp(datas->cureltname, "URLBase") )
		dstmember = datas->urlbase;

	else if(datas->state<1)
	{

		if(!strncmp(datas->curmaineltname,"argument",l)){
			struct IGDdevice * dev = datas->dlist[datas->curdevice];
			struct IGDservice * srv = dev->slist[dev->curservice];
			struct IGDaction * act = srv->alist[srv->curaction];
			struct IGDargument * arg = act->alist[act->curargument];

			if(!arg){
					arg	= (struct IGDargument *) malloc(sizeof(struct IGDargument));
					memset(arg,0,sizeof(struct IGDargument));
					act->alist[act->curargument] = arg;
			}

			if(!strcmp(datas->cureltname, "name") ){
				dstmember = arg->name;
			}
		
		}else
		if(!strncmp(datas->curmaineltname,"action",l)){	
			struct IGDdevice * dev = datas->dlist[datas->curdevice];
			struct IGDservice * srv = dev->slist[dev->curservice];
			struct IGDaction * act = srv->alist[srv->curaction];

			if(!act){
					act	= (struct IGDaction *) malloc(sizeof(struct IGDaction));
					memset(act,0,sizeof(struct IGDaction));
					srv->alist[srv->curaction] = act;
			}
			if(!strcmp(datas->cureltname, "name") )
				dstmember = act->name;
		}else
		if(!strncmp(datas->curmaineltname,"service",l)){
			struct IGDdevice * dev = datas->dlist[datas->curdevice];
			struct IGDservice * srv = dev->slist[dev->curservice];

			if(!srv){
					srv	= (struct IGDservice *) malloc(sizeof(struct IGDservice));
					memset(srv,0,sizeof(struct IGDservice));
					dev->slist[dev->curservice] = srv;
			}

			if( !strcmp(datas->cureltname, "serviceType") )
				dstmember = srv->servicetype;
			else if( !strcmp(datas->cureltname, "controlURL") )
				dstmember = srv->controlurl;
			else if( !strcmp(datas->cureltname, "eventSubURL") )
				dstmember = srv->eventsuburl;
			else if( !strcmp(datas->cureltname, "SCPDURL") )
				dstmember = srv->scpdurl;


		}else
		if(!strncmp(datas->curmaineltname,"device",l)){

			struct IGDdevice * dev = datas->dlist[datas->curdevice];
			if(!dev){
					dev = (struct IGDdevice *) malloc(sizeof(struct IGDdevice));
					memset(dev,0,sizeof(struct IGDdevice));
					datas->dlist[datas->curdevice]  = dev;
			}

			if( !strcmp(datas->cureltname, "deviceType") )
				dstmember = dev->devicetype;
			else
			if( !strcmp(datas->cureltname, "friendlyName") )
				dstmember = dev->friendlyname;
			else
			if( !strcmp(datas->cureltname, "manufacturer") )
				dstmember = dev->manufacturer;
			else
			if( !strcmp(datas->cureltname, "manufacturerURL") )
				dstmember = dev->manufacturerurl;
			else
			if( !strcmp(datas->cureltname, "modelDescription") )
				dstmember = dev->modeldescription;
			else
			if( !strcmp(datas->cureltname, "modelName") )
				dstmember = dev->modelname;
			else
			if( !strcmp(datas->cureltname, "modelNumber") )
				dstmember = dev->modelnumber;
			else
			if( !strcmp(datas->cureltname, "modelURL") )
				dstmember = dev->modelurl;
			else
			if( !strcmp(datas->cureltname, "serialNumber") )
				dstmember = dev->serialnumber;
			else
			if( !strcmp(datas->cureltname, "UDN") )
				dstmember = dev->udn;
		}else{


		}

	}
	if(dstmember)
	{
		if(l>63)
			l = 63;
		memcpy(dstmember, data, l);
		dstmember[l] = '\0';
	}
}

void printIGD(struct IGDdatas * datas)
{
	int d,s,a,p;
	int ignored = 0;
	printf("urlbase = %s\n", datas->urlbase);

	printf("Devices found = %u\n",datas->devices);

	for(d=0;d<datas->devices;d++){
		struct IGDdevice * dev = datas->dlist[d];

		if(!dev || !dev->devicetype[0]){
			ignored++;
			continue;								
		}
		printf("device type = %s\n", dev->devicetype);
		printf("friendly name = %s\n", dev->friendlyname);
		printf("manufacturer = %s\n", dev->manufacturer);
		printf("manufacturer url = %s\n", dev->manufacturerurl);
		printf("model name = %s\n", dev->modelname);
		printf("model number = %s\n", dev->modelnumber);
		printf("model description = %s\n", dev->modeldescription);
		printf("model serial = %s\n", dev->serialnumber);
		printf("model url = %s\n", dev->modelurl);
		printf("udn = %s\n", dev->udn);

		printf("Services found: %u\n", dev->services);
		for(s=0;s<dev->services;s++)
			{
				struct IGDservice * srv = dev->slist[s];
				if(!srv)
					continue;
				printf("  Service: %2.2u ",s);
				printf("  service type = %s\n", srv->servicetype);
				printf("  control url = %s\n", srv->controlurl);
				printf("  event sub url = %s\n", srv->eventsuburl);
				printf("  scpd url = %s\n", srv->scpdurl);
				
				printf("  Actions found: %u\n", srv->actions);
				for(a=0;a<srv->actions;a++)
				{
					struct IGDaction * act = srv->alist[a];

					if(!act)
						continue;

					printf("   %s  - ",act->name);

					for(p=0;p<act->arguments;p++){
						struct IGDargument * arg = act->alist[p];
						
						if(!arg)
							continue;

						printf("%s ", arg->name);
					}
						printf("\n");

				}
				printf("\n");
			}	
			printf("\n");
	}

	printf("Ignore %u devices without type!\n",ignored);
}



void freeIGD(struct IGDdatas* datas)
{
	int d,s,a,p;

	if(!datas)
		return;

	for(d=0;d<datas->devices;d++){
		struct IGDdevice * dev = datas->dlist[d];

		if(!dev){
			continue;								
		}
		for(s=0;s<dev->services;s++)
			{
				struct IGDservice * srv = dev->slist[s];
				
				if(!srv)
					continue;
				
				for(a=0;a<srv->actions;a++)
				{
					struct IGDaction * act = srv->alist[a];

					if(!act)
						continue;

					for(p=0;p<act->arguments;p++){
						struct IGDargument * arg = act->alist[p];
						
						if(!arg)
							continue;
						
						free(arg);
						act->alist[p] = NULL;
					}
					free(act);
					srv->alist[a] = NULL;
				}
				free(srv);
				dev->slist[s] = NULL;
			}	
			free(dev);
			datas->dlist[d] = NULL;
	}
	free(datas);
}



