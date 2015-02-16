.PHONY= default all
CLAGS+= -lsndfile

default: all

all: lsampadd test_lsamp 

test_lsamp: test_lsamp.c lsamp.o lsamp.h
	gcc -Wall -lsndfile -o $@ $< lsamp.o
memtest: test_lsamp
	valgrind --leak-check=yes test_lsamp

lsampadd: lsamp.o lsampadd.c
	gcc -Wall lsampadd.c -lsndfile -o$@ lsamp.o

out.smp: out.header out.bin
	cat out.header out.bin > out.smp

lsamp_seq: lsamp_seq.c lsamp.o
	gcc -Wall lsamp_seq.c -lsndfile -o $@ lsamp.o

clean:
	rm -rf test_lsamp lsamp.o lsampout.wav lsampadd
