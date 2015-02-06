.PHONY= default
CLAGS+= -lsndfile

default: test_lsamp

test_lsamp: test_lsamp.c lsamp.o lsamp.h
	gcc -lsndfile -o $@ $< lsamp.o
memtest: test_lsamp
	valgrind --leak-check=yes test_lsamp


out.smp: out.header out.bin
	cat out.header out.bin > out.smp

clean:
	rm -rf test_lsamp lsamp.o lsampout.wav
