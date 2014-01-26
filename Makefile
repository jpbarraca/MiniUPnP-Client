# $Id: Makefile,v 1.10 2006/05/27 10:44:30 nanard Exp $
CC = gcc
CFLAGS = -Wall -g

all:	upnpc libminiupnpc.a

clean:
		rm upnpc *.o *~

snapshot:
		tar cvzf ../mupnp-`date +"%Y%m%d"`.tar.gz LICENCE README README.upnpc *.c *.h Makefile

install:
	  install -m 555 upnpc /usr/local/bin

libminiupnpc.a:	miniwget.o minixml.o igd_desc_parse.o minisoap.o miniupnpc.o upnpreplyparse.o upnpcommands.o
	$(AR) cr $@ $?

upnpc:	upnpc.o libminiupnpc.a

minixml.o:	minixml.c minixml.h

upnpc.o:	upnpc.c miniwget.h minisoap.h miniupnpc.h igd_desc_parse.h upnpreplyparse.h

miniwget.o:	miniwget.c miniwget.h

minisoap.o:	minisoap.c minisoap.h

miniupnpc.o:	miniupnpc.c miniupnpc.h minisoap.h miniwget.h minixml.h

igd_desc_parse.o:	igd_desc_parse.c igd_desc_parse.h

upnpreplyparse.o:	upnpreplyparse.c upnpreplyparse.h minixml.h

upnpcommands.o:	upnpcommands.c upnpcommands.h upnpreplyparse.h miniupnpc.h

