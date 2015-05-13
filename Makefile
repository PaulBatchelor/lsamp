.PHONY: all default

default: all

all: lsamp liblsamp.a

CFLAGS=-g -lsndfile -lsqlite3 -I ./

lsamp.o: lsamp.c
	#gcc -g -DLS_DEBUG -c lsamp.c
	gcc -g -c lsamp.c

liblsamp.a: lsamp.o
	ar rcs liblsamp.a lsamp.o

create_lsamp_file: t/create_lsamp_file.c lsamp.o
	gcc $(CFLAGS) $< -o $@ lsamp.o

lsamp: lsamp_cli.c lsamp.o
	gcc $(CFLAGS) $< -o $@ lsamp.o

install: 
	cp liblsamp.a /usr/local/lib
	cp lsamp.h /usr/local/include

uninstall:
	rm -f /usr/local/lib/liblsamp.a
	rm -f /usr/local/lib/lsamp.h

clean: 
	rm -f liblsamp.a lsamp.o lsamp
