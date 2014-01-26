/* $Id: upnpcommands.c,v 1.3 2006/05/27 10:44:31 nanard Exp $ */
/* Project : miniupnp
 * Author : Thomas Bernard
 * Copyright (c) 2005 Thomas Bernard
 * This software is subject to the conditions detailed in the
 * LICENCE file provided in this distribution.
 * */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "upnpcommands.h"
#include "miniupnpc.h"
//IGD

//HostConfigManagement


//WANCommonInterface

//WANDSLLink
//
int UPNP_GetGeneralVariable(const char* controlURL, 
														const char* servicetype, 
														const char* variable,
														const struct UPNParg* arguments)
{
	char buffer[4096];
	int bufsize = 4096;

	if(!controlURL || !servicetype || !variable)
		return -1;

	simpleUPnPcommand(-1, controlURL, servicetype, variable, arguments, buffer, &bufsize);
	DisplayNameValueList(buffer, bufsize);
	return COMMAND_OK; 
}
//Layer3Forwarding
/* UPNP_GetStatusInfo() call the corresponding UPNP method
 * returns the current status and uptime */
void UPNP_GetStatusInfo(const char * controlURL,
												const char * servicetype,
												char * status, 
												unsigned int * uptime)
{
	struct NameValueParserData pdata;
	char buffer[4096];
	int bufsize = 4096;
	char * p;
	char* up;

	if(!status && !uptime)
		return;

	simpleUPnPcommand(-1, controlURL, servicetype, "GetStatusInfo", 0, buffer, &bufsize);
	ParseNameValue(buffer, bufsize, &pdata);
	//DisplayNameValueList(buffer, bufsize);
	up = GetValueFromNameValueList(&pdata, "NewUptime");
	p = GetValueFromNameValueList(&pdata, "NewConnectionStatus");

	if(status)
	{
		if(p){
			strncpy(status, p, 64 );
			status[64] = '\0';
		}else
			status[0]= '\0';
	}

	if(uptime){
		if(p)
			sscanf(up,"%u",uptime);
		else
			uptime = 0;
	}

	ClearNameValueList(&pdata);
}

/* UPNP_Connect() call the corresponding UPNP method
 * returns the status */
void UPNP_RequestConnection(const char * controlURL,
                                const char * servicetype,
																char * status)
{
	struct NameValueParserData pdata;
	char buffer[4096];
	int bufsize = 4096;
	char * p;

	simpleUPnPcommand(-1, controlURL, servicetype, "RequestConnection", 0, buffer, &bufsize);
	ParseNameValue(buffer, bufsize, &pdata);
	p = GetValueFromNameValueList(&pdata, "NewConnectionType");
	//p = GetValueFromNameValueList(&pdata, "NewPossibleConnectionTypes");
	// PossibleConnectionTypes will have several values....
	if(status)
	{
		if(p){
			strncpy(status, p, 64 );
			status[63] = '\0';
		}	else
			status[0] = '\0';
	}

	ClearNameValueList(&pdata);
}

/* UPNP_GetConnectionTypeInfo() call the corresponding UPNP method
 * returns the connection type */
void UPNP_GetConnectionTypeInfo(const char * controlURL,
                                const char * servicetype,
																char * connectionType)
{
	struct NameValueParserData pdata;
	char buffer[4096];
	int bufsize = 4096;
	char * p;

	if(!connectionType)
		return;


	simpleUPnPcommand(-1, controlURL, servicetype, "GetConnectionTypeInfo", 0, buffer, &bufsize);
	ParseNameValue(buffer, bufsize, &pdata);
	p = GetValueFromNameValueList(&pdata, "NewConnectionType");
	//p = GetValueFromNameValueList(&pdata, "NewPossibleConnectionTypes");
	// PossibleConnectionTypes will have several values....
	if(connectionType)
	{
		if(p){
			strncpy(connectionType, p, 64 );
			connectionType[63] = '\0';
		}	else
			connectionType[0] = '\0';
	}
	ClearNameValueList(&pdata);
}

/* UPNP_GetLinkLayerMaxBitRate() call the corresponding UPNP method.
 * Returns 2 values: Downloadlink bandwidth and Uplink bandwidth. One of the values can be null */
void UPNP_GetLinkLayerMaxBitRates(const char * controlURL, const char * servicetype, unsigned int * bitrateDown, unsigned int* bitrateUp)
{
	struct NameValueParserData pdata;
	char buffer[4096];
	int bufsize = 4096;
	char * down;
	char* up;

	if(!bitrateDown && !bitrateUp)
		return;

	simpleUPnPcommand(-1, controlURL, servicetype, "GetLinkLayerMaxBitRates", 0, buffer, &bufsize);
//	DisplayNameValueList(buffer, bufsize);
	ParseNameValue(buffer, bufsize, &pdata);
	down = GetValueFromNameValueList(&pdata, "NewDownstreamMaxBitRate");
	up = GetValueFromNameValueList(&pdata, "NewUpstreamMaxBitRate");

	if(bitrateDown)
	{
		if(down)
			sscanf(down,"%u",bitrateDown);
		else
			*bitrateDown = 0;
	}

	if(bitrateUp)
	{
		if(up)
			sscanf(up,"%u",bitrateUp);
		else
			*bitrateUp = 0;
	}
	ClearNameValueList(&pdata);
}


/* UPNP_GetExternalIPAddress() call the corresponding UPNP method.
 * if the third arg is not null the value is copied to it.
 * at least 16 bytes must be available */
void UPNP_GetExternalIPAddress(const char * controlURL, const char * servicetype, char * extIpAdd)
{
	struct NameValueParserData pdata;
	char buffer[4096];
	int bufsize = 4096;
	char * p;

	if(!extIpAdd)
		return;

	simpleUPnPcommand(-1, controlURL, servicetype, "GetExternalIPAddress", 0, buffer, &bufsize);
	/*fd = simpleUPnPcommand(fd, controlURL, data.servicetype, "GetExternalIPAddress", 0, buffer, &bufsize);*/
	//DisplayNameValueList(buffer, bufsize);
	ParseNameValue(buffer, bufsize, &pdata);
	//printf("external ip = %s\n", GetValueFromNameValueList(&pdata, "NewExternalIPAddress") );
	p = GetValueFromNameValueList(&pdata, "NewExternalIPAddress");

	if(p){
		strncpy(extIpAdd, p, 16 );
		extIpAdd[15] = '\0';
	}else
		extIpAdd[0] = '\0';

	ClearNameValueList(&pdata);
}

void UPNP_AddPortMapping(const char * controlURL, const char * servicetype,
                         const char * extPort,
						 const char * inPort,
						 const char * inClient)
{
	struct UPNParg * AddPortMappingArgs;
	char buffer[4096];
	int bufsize = 4096;

	if(!inPort || !inClient)
		return;

	AddPortMappingArgs = calloc(9, sizeof(struct UPNParg));
	AddPortMappingArgs[0].elt = "NewRemoteHost";
	AddPortMappingArgs[1].elt = "NewExternalPort";
	AddPortMappingArgs[1].val = extPort;
	AddPortMappingArgs[2].elt = "NewProtocol";
	AddPortMappingArgs[2].val = "TCP";
	AddPortMappingArgs[3].elt = "NewInternalPort";
	AddPortMappingArgs[3].val = inPort;
	AddPortMappingArgs[4].elt = "NewInternalClient";
	AddPortMappingArgs[4].val = inClient;
	AddPortMappingArgs[5].elt = "NewEnabled";
	AddPortMappingArgs[5].val = "1";
	AddPortMappingArgs[6].elt = "NewPortMappingDescription";
	AddPortMappingArgs[6].val = "testmachinchose";	// TODO: change this
	AddPortMappingArgs[7].elt = "NewLeaseDuration";
	AddPortMappingArgs[7].val = "0";
	simpleUPnPcommand(-1, controlURL, servicetype, "AddPortMapping", AddPortMappingArgs, buffer, &bufsize);
	/*fd = simpleUPnPcommand(fd, controlURL, data.servicetype, "AddPortMapping", AddPortMappingArgs, buffer, &bufsize);*/
	//DisplayNameValueList(buffer, bufsize);
	free(AddPortMappingArgs);
}

void UPNP_DeletePortMapping(const char * controlURL, const char * servicetype,
                            const char * extPort)
{
	//struct NameValueParserData pdata;
	struct UPNParg * DeletePortMappingArgs;
	char buffer[4096];
	int bufsize = 4096;

	if(!extPort)
		return;	

	DeletePortMappingArgs = calloc(4, sizeof(struct UPNParg));
	DeletePortMappingArgs[0].elt = "NewRemoteHost";
	DeletePortMappingArgs[1].elt = "NewExternalPort";
	DeletePortMappingArgs[1].val = extPort;
	DeletePortMappingArgs[2].elt = "NewProtocol";
	DeletePortMappingArgs[2].val = "TCP";
	simpleUPnPcommand(-1, controlURL, servicetype,
	                  "DeletePortMapping",
					  DeletePortMappingArgs, buffer, &bufsize);
	DisplayNameValueList(buffer, bufsize);
	free(DeletePortMappingArgs);
}

void UPNP_GetPortMappingNumberOfEntries(const char * controlURL, const char * servicetype, unsigned int * numEntries)
{
	struct NameValueParserData pdata;
	char buffer[4096];
	int bufsize = 4096;
	char* p;
	simpleUPnPcommand(-1, controlURL, servicetype, "GetPortMappingNumberOfEntries", 0, buffer, &bufsize);
//	DisplayNameValueList(buffer, bufsize);
	ParseNameValue(buffer, bufsize, &pdata);
	p = GetValueFromNameValueList(&pdata, "NewDownstreamMaxBitRate");

	if(numEntries && p)
	{
			sscanf(p,"%u",numEntries);
	}
	ClearNameValueList(&pdata);
}

/* UPNP_GetSpecificPortMappingEntry retrieves an existing port mapping
 * the result is returned in the intClient and intPort strings
 * please provide 16 and 6 bytes of data */
void UPNP_GetSpecificPortMappingEntry(const char * controlURL, const char * servicetype, const char * extPort, char * intClient, char * intPort)
{
	struct NameValueParserData pdata;
	struct UPNParg * GetPortMappingArgs;
	char buffer[4096];
	int bufsize = 4096;
	char * p;

	if(!intPort && !intClient && !extPort)
		return;

	GetPortMappingArgs = calloc(4, sizeof(struct UPNParg));
	GetPortMappingArgs[0].elt = "NewRemoteHost";
	GetPortMappingArgs[1].elt = "NewExternalPort";
	GetPortMappingArgs[1].val = extPort;
	GetPortMappingArgs[2].elt = "NewProtocol";
	GetPortMappingArgs[2].val = "TCP";
	simpleUPnPcommand(-1, controlURL, servicetype,
	                  "GetSpecificPortMappingEntry",
					  GetPortMappingArgs, buffer, &bufsize);
	/*fd = simpleUPnPcommand(fd, controlURL, data.servicetype, "GetSpecificPortMappingEntry", AddPortMappingArgs, buffer, &bufsize); */
	//DisplayNameValueList(buffer, bufsize);
	ParseNameValue(buffer, bufsize, &pdata);
	p = GetValueFromNameValueList(&pdata, "NewInternalClient");

	if(intClient)
	{
		if(p){
			strncpy(intClient, p, 16);
			intClient[15] = '\0';
		}else
			intClient[0] = '\0';
	}

	p = GetValueFromNameValueList(&pdata, "NewInternalPort");
	if(intPort)
	{
		if(p){
			strncpy(intPort, p, 6);
			intPort[5] = '\0';
		}else
			intPort[0] = '\0';
	}

	ClearNameValueList(&pdata);
	free(GetPortMappingArgs);
}


