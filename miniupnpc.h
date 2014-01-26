/* $Id: miniupnpc.h,v 1.5 2006/05/27 10:44:30 nanard Exp $ */
#ifndef __MINIUPNPC_H__
#define __MINIUPNPC_H__
#include "igd_desc_parse.h"

struct UPNParg { const char * elt; const char * val; };

int simpleUPnPcommand(int, const char *, const char *,
                      const char *, struct UPNParg *,
                      char *, int *);

char * upnpDiscover(int);

void parserootdesc(const char *, int, struct IGDdatas *);
void parseservicedesc(const char* buffer, int bufsize, struct IGDdatas* data, const unsigned devid, const unsigned sid);
struct UPNPUrls { char * controlURL; char * ipcondescURL; };

void GetUPNPUrls(struct UPNPUrls *, struct IGDdatas *, const char* servicetype, const char *);

void FreeUPNPUrls(struct UPNPUrls *);

#endif

