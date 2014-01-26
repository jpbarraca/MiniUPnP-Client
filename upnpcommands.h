/* $Id: upnpcommands.h,v 1.2 2006/05/13 08:45:12 nanard Exp $ */
#ifndef __UPNPCOMMANDS_H__
#define __UPNPCOMMANDS_H__

#include "upnpreplyparse.h"
#include "miniupnpc.h"

#define COMMAND_OK 					0
#define COMMAND_ERROR 				1
#define COMMAND_PARAM_ERROR 	2
int UPNP_GetGeneralVariable(const char* controlURL,const char* servicetype,const char* variable, const struct UPNParg* arguments);
int UPNP_GetLayer1MaxUpstreamBitRate(const char *controlURL, const char *servicetype, char *bandwidth);
void UPNP_GetStatusInfo(const char *controlURL, const char *servicetype, char *status, unsigned int *uptime);
void UPNP_RequestConnection(const char *controlURL, const char *servicetype, char *status);
void UPNP_GetConnectionTypeInfo(const char *controlURL, const char *servicetype, char *connectionType);
void UPNP_GetLinkLayerMaxBitRates(const char *controlURL, const char *servicetype, unsigned int *bitrateDown, unsigned int *bitrateUp);
void UPNP_GetExternalIPAddress(const char *controlURL, const char *servicetype, char *extIpAdd);
void UPNP_AddPortMapping(const char *controlURL, const char *servicetype, const char *extPort, const char *inPort, const char *inClient);
void UPNP_DeletePortMapping(const char *controlURL, const char *servicetype, const char *extPort);
void UPNP_GetPortMappingNumberOfEntries(const char *controlURL, const char *servicetype, unsigned int *numEntries);
void UPNP_GetSpecificPortMappingEntry(const char *controlURL, const char *servicetype, const char *extPort, char *intClient, char *intPort);


#endif
