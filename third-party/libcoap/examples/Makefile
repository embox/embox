# Makefile for libcoap
#
# Copyright (C) 2010,2011 Olaf Bergmann <bergmann@tzi.org>
#
# This file is part of the CoAP library libcoap. Please see
# README for terms of use. 

# the library's version
VERSION:=4.1.1

# tools

SHELL = /bin/sh
MKDIR = mkdir

abs_builddir = /home/parallels/Documents/embox/build/extbld/third_party/lib/libcoap/libcoap-4.1.1/examples
top_builddir = ..
top_srcdir = ..
# files and flags
PROGRAMS:=coap-server coap-client rd etsi_iot_01 #tiny
SOURCES:= tiny.c client.c server.c rd.c etsi_iot_01.c
OBJECTS:= $(patsubst %.c, %.o, $(SOURCES))
#CFLAGS:=-g -Wall -ansi -pedantic -I..
CFLAGS:=-g -Wall -g -O2
CPPFLAGS:=-I$(top_srcdir) -Drpl_malloc=malloc -DWITH_POSIX
DISTDIR?=$(top_builddir)/libcoap-4.1.1
FILES:=Makefile.in $(SOURCES)
LDFLAGS:=-L$(top_builddir)
LDLIBS:=-lcoap 
libcoap =$(top_builddir)/libcoap.a

.PHONY: clean distclean

.SUFFIXES:
.SUFFIXES:      .c .o

all:	$(PROGRAMS)

check:	
	echo DISTDIR: $(DISTDIR)
	echo top_builddir: $(top_builddir)

tiny:	tiny.o $(libcoap)
	$(CC) -o $@ $< $(LDFLAGS)

coap-client: client.o $(libcoap)
	$(CC) -o $@ $< $(LDFLAGS) $(LDLIBS)

coap-server:	server.o $(libcoap)
	$(CC) -o $@ $< $(LDFLAGS) $(LDLIBS)

clean:
	@rm -f $(PROGRAMS) $(OBJECTS)

distclean:	clean
	@rm -rf $(DISTDIR)
	@rm -f *~ 

dist:	$(FILES)
	test -d $(DISTDIR)/examples || mkdir $(DISTDIR)/examples
	cp $(FILES) $(DISTDIR)/examples
