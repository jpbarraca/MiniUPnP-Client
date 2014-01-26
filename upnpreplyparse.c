/* $Id: upnpreplyparse.c,v 1.4 2006/05/27 10:44:31 nanard Exp $ */
/* Project : miniupnp
 * Author : Thomas Bernard
 * Copyright (c) 2005 Thomas Bernard
 * This software is subject to the conditions detailed in
 * the LICENCE file provided with this distribution. */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "upnpreplyparse.h"
#include "minixml.h"

void NameValueParserStartElt(void * d, const char * name, int l)
{
    struct NameValueParserData * data = (struct NameValueParserData *)d;
    if(l>63)
        l = 63;
    memcpy(data->curelt, name, l);
    data->curelt[l] = '\0';
}

void NameValueParserEndElt(void * d, const char * name, int l)
{
}

void NameValueParserGetData(void * d, const char * datas, int l)
{
    struct NameValueParserData * data = (struct NameValueParserData *)d;
    struct NameValue * nv;
    nv = malloc(sizeof(struct NameValue));
    if(l>63)
        l = 63;
    //strcpy(nv->name, data->curelt);
    strncpy(nv->name, data->curelt, 64);
	nv->name[63] = '\0';
    memcpy(nv->value, datas, l);
    nv->value[l] = '\0';
    LIST_INSERT_HEAD( &(data->head), nv, entries);
}

void ParseNameValue(const char * buffer, int bufsize,
                    struct NameValueParserData * data)
{
    struct xmlparser parser;
    LIST_INIT(&(data->head));
    // objet xmlparser
    parser.xmlstart = buffer;
    parser.xmlsize = bufsize;
    parser.data = data;
    parser.starteltfunc = NameValueParserStartElt;
    parser.endeltfunc = NameValueParserEndElt;
    parser.datafunc = NameValueParserGetData;
    parsexml(&parser); // goon !
}

void ClearNameValueList(struct NameValueParserData * pdata)
{
    struct NameValue * nv;
    while((nv = pdata->head.lh_first) != NULL)
    {
        LIST_REMOVE(nv, entries);
        free(nv);
    }
}

char * GetValueFromNameValueList(struct NameValueParserData * pdata, const char * Name)
{
    struct NameValue * nv;
    char * p = NULL;
    for(nv = pdata->head.lh_first;
        (nv != NULL) && (p == NULL);
        nv = nv->entries.le_next)
    {
        if(strcmp(nv->name, Name) == 0)
            p = nv->value;
    }
    return p;
}

/* debug all-in-one function 
 * do parsing then display to stdout */
void DisplayNameValueList(char * buffer, int bufsize)
{
    struct NameValueParserData pdata;
    struct NameValue * nv;
    ParseNameValue(buffer, bufsize, &pdata);
    for(nv = pdata.head.lh_first;
        nv != NULL;
        nv = nv->entries.le_next)
    {
        printf(" %s = %s\n", nv->name, nv->value);
    }
    ClearNameValueList(&pdata);
}

