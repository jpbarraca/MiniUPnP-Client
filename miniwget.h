/* Project : miniupnp
 * Author : Thomas Bernard
 * Copyright (c) 2005 Thomas Bernard
 * This software is subject to the conditions detailed in the
 * LICENCE file provided in this distribution.
 * */
#ifndef __MINIWGET_H__
#define __MINIWGET_H__

void * miniwget(const char *, int *);

int parseURL(const char *, char *, unsigned short *, char * *);


#endif

