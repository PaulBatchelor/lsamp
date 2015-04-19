CFLAGS=-g -lsndfile -lsqlite3 -I ./

lsamp.o: lsamp.c
	gcc -g -DLS_DEBUG -c lsamp.c

create_lsamp_file: t/create_lsamp_file.c lsamp.o
	gcc $(CFLAGS) $< -o $@ lsamp.o

lsamp: lsamp_cli.c lsamp.o
	gcc $(CFLAGS) $< -o $@ lsamp.o
