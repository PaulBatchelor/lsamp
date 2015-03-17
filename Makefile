.PHONY= default all smp_files
CLAGS+= -lsndfile

SMP_FILES = tr909.smp

default: all

all: lsmp test_lsamp 

smp_files: $(addprefix smp/, $(SMP_FILES))

test_lsamp: test_lsamp.c main.o lsamp.h
	gcc -Wall -lsndfile -o $@ $< main.o
memtest: test_lsamp
	valgrind --leak-check=yes test_lsamp

liblsamp.a: main.c
	ar rcs $@ main.o

lsmp: lsmp.c main.o
	gcc $< -Wall -lsndfile -o$@ main.o 

main.o: main.c
	gcc -c $< 

out.smp: out.header out.bin
	cat out.header out.bin > out.smp

tests/lsamp_combine: tests/lsamp_combine.c main.o
	gcc -g -Wall $< -lsndfile -o $@ main.o

lsamp_repeater: lsamp_repeater.c main.o smp_files
	gcc -g -Wall $< -lsndfile -o $@ main.o

clean:
	rm -rf test_lsamp main.o lsampout.wav lsmp lsamp_repeater
	rm -rf $(addprefix smp/, $(SMP_FILES))

smp/tr909.smp: lsmp
	cd smp; sh create_tr909.sh
