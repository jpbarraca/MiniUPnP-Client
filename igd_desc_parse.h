/* Project : miniupnp
 * Author : Thomas Bernard
 * Copyright (c) 2005 Thomas Bernard
 * This software is subject to the conditions detailed in the
 * LICENCE file provided in this distribution.
 * */
#ifndef __IGD_DESC_PARSE_H__
#define __IGD_DESC_PARSE_H__

/* Pour le parsing des documents de descriptions UPNP
 * des Internet Gateway Devices */

struct IGDargument{
	char name[64];
	char type[64];
};

struct IGDaction {
	char name[64];
	unsigned char arguments;
	unsigned char curargument;
	struct IGDargument *alist[20];
};

struct IGDservice{
	char controlurl[64];
	char eventsuburl[64];
	char scpdurl[64];
	char servicetype[64];

	unsigned char actions;
	unsigned char curaction;
	struct IGDaction *alist[40];
};

struct IGDdevice {
	char devicetype[64];
	char friendlyname[64];
	char manufacturer[64];
	char manufacturerurl[64];
	char modelname[64];
	char modeldescription[64];
	char modelnumber[64];
	char modelurl[64];
	char serialnumber[64];
	char udn[64];

	unsigned char	services;
	unsigned char curservice;
	struct IGDservice *slist[20];
};

struct IGDdatas {
	char cureltname[64];
	char curmaineltname[64];
	char urlbase[64];
	int level;
	int state;
	unsigned char devices;
	unsigned char curdevice;
	struct IGDdevice *dlist[10];
};


void IGDstartelt(void *, const char *, int);
void IGDendelt(void *, const char *, int);
void IGDdata(void *, const char *, int);
void printIGD(struct IGDdatas *);
void freeIGD(struct IGDdatas*);
#endif

