/*
 * minixml.c : the minimum size a xml parser can be !
 * */
/* Project : miniupnp
 * Author : Thomas Bernard
 * Copyright (c) 2005 Thomas Bernard
 * This software is subject to the conditions detailed in the
 * LICENCE file provided in this distribution.
 * */
#include "minixml.h"

/* parseatt : used to parse the argument list
 * return 0 (false) in case of success and -1 (true) if the end
 * of the xmlbuffer is reached.
 * At the moment, nothing is done with the attributes found */
int parseatt(struct xmlparser * p)
{
	char attname[64];
	char attvalue[64];
	while(p->xml < p->xmlend)
	{
		if(*p->xml=='/' || *p->xml=='>')
			return 0;
		if( !IS_WHITE_SPACE(*p->xml) )
		{
			int j = 0;
			char sep;
			while(*p->xml!='=' && !IS_WHITE_SPACE(*p->xml) )
			{
				attname[j++] = *(p->xml++);
				if(p->xml >= p->xmlend)
					return -1;
			}
			attname[j] = '\0';
			while(*(p->xml++) != '=')
			{
				if(p->xml >= p->xmlend)
					return -1;
			}
			while(IS_WHITE_SPACE(*p->xml))
			{
				p->xml++;
				if(p->xml >= p->xmlend)
					return -1;
			}
			sep = *p->xml;
			j = 0;
			if(sep=='\'' || sep=='\"')
			{
				p->xml++;
				if(p->xml >= p->xmlend)
					return -1;
				while(*p->xml != sep)
				{
					attvalue[j++] = *(p->xml++);
					if(p->xml >= p->xmlend)
						return -1;
				}
			}
			else
			{
				while(   !IS_WHITE_SPACE(*p->xml)
					  && *p->xml != '>' && *p->xml != '/')
				{
					attvalue[j++] = *(p->xml++);
					if(p->xml >= p->xmlend)
						return -1;
				}
			}
			attvalue[j]='\0';
			//printf("'%s'='%s' ", attname, attvalue);
		}
		p->xml++;
	}
	return -1;
}

/* parseelt parse the xml stream and
 * call the callback functions when needed... */
void parseelt(struct xmlparser * p)
{
	int i;
	const char * elementname;
	while(p->xml < (p->xmlend - 1))
	{
		if((p->xml)[0]=='<' && (p->xml)[1]!='?')
		{
			i = 0; elementname = ++p->xml;
			while( !IS_WHITE_SPACE(*p->xml)
				  && (*p->xml!='>') && (*p->xml!='/')
				 )
			{
				i++; p->xml++;
				if (p->xml >= p->xmlend)
					return;
			}
			if(i>0)
			{
				p->starteltfunc(p->data, elementname, i);
				if(parseatt(p))
					return;
				if(*p->xml!='/')
				{
					const char * data;
					i = 0; data = ++p->xml;
					if (p->xml >= p->xmlend)
						return;
					while( IS_WHITE_SPACE(*p->xml) )
					{
						p->xml++;
						if (p->xml >= p->xmlend)
							return;
					}
					while(*p->xml!='<')
					{
						i++; p->xml++;
						if (p->xml >= p->xmlend)
							return;
					}
					if(i>0)
						p->datafunc(p->data, data, i);
				}
			}
			else if(*p->xml == '/')
			{
				i = 0; elementname = ++p->xml;
				if (p->xml >= p->xmlend)
					return;
				while((*p->xml != '>'))
				{
					i++; p->xml++;
					if (p->xml >= p->xmlend)
						return;
				}
				p->endeltfunc(p->data, elementname, i);
				p->xml++;
			}
		}
		else
		{
			p->xml++;
		}
	}
}

// fonction a appeler avec en parametre un "parser" initialisé
void parsexml(struct xmlparser * parser)
{
	parser->xml = parser->xmlstart;
	parser->xmlend = parser->xmlstart + parser->xmlsize;
	parseelt(parser);
}


