/* 
 * minimal xml parser
 *
 * Project : miniupnp
 * Author : Thomas Bernard
 * Copyright (c) 2005 Thomas Bernard
 * This software is subject to the conditions detailed in the
 * LICENCE file provided in this distribution.
 * */
#ifndef __MINIXML_H__
#define __MINIXML_H__
//#define XML_ELT_MAX_LENGTH (64)
#define IS_WHITE_SPACE(c) ((c==' ') || (c=='\t') || (c=='\r') || (c=='\n'))

struct xmlparser {
	const char *xmlstart;
	const char *xmlend;
	const char *xml;	// pointeur sur le caractere courrant
	int xmlsize;
	void * data;
	void (*starteltfunc) (void *, const char *, int);
	void (*endeltfunc) (void *, const char *, int);
	void (*datafunc) (void *, const char *, int);
};

// fonction a appeler avec en parametre un "parser" initialisé
void parsexml(struct xmlparser *);

#endif

