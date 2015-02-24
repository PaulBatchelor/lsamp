.PHONY= default all
CLAGS+= -lsndfile

default: all

all: lsmp test_lsamp 

test_lsamp: test_lsamp.c main.o lsamp.h
	gcc -Wall -lsndfile -o $@ $< main.o
memtest: test_lsamp
	valgrind --leak-check=yes test_lsamp

liblsamp.a: main.c
	ar rcs $< main.o

lsmp: lsmp.c main.o
	gcc $< -Wall -lsndfile -o$@ main.o 

main.o: main.c
	gcc -c $< 

out.smp: out.header out.bin
	cat out.header out.bin > out.smp

tests/lsamp_combine: tests/lsamp_combine.c main.o
	gcc -g -Wall $< -lsndfile -o $@ main.o

lsamp_repeater: lsamp_repeater.c main.o
	gcc -g -Wall $< -lsndfile -o $@ main.o

clean:
	rm -rf test_lsamp main.o lsampout.wav lsmp lsamp_repeater
