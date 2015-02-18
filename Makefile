.PHONY= default all
CLAGS+= -lsndfile

default: all

all: lsmp test_lsamp 

test_lsamp: test_lsamp.c lsamp.o lsamp.h
	gcc -Wall -lsndfile -o $@ $< lsamp.o
memtest: test_lsamp
	valgrind --leak-check=yes test_lsamp

lsmp: lsmp.c lsamp.o
	gcc -Wall $< -lsndfile -o$@ lsamp.o

out.smp: out.header out.bin
	cat out.header out.bin > out.smp

lsamp_repeater: lsamp_repeater.c lsamp.o
	gcc -g -Wall $< -lsndfile -o $@ lsamp.o

clean:
	rm -rf test_lsamp lsamp.o lsampout.wav lsmp lsamp_repeater
