CLAGS+= -lsndfile
lsamp.o : lsamp.c lsamp.h
	gcc -c lsamp.c
test_lsamp : gen_header.c lsamp.o
	gcc -lsndfile -o $@ $< lsamp.o
memtest: test_lsamp
	valgrind --leak-check=yes test_lsamp
clean:
	rm -rf test_lsamp lsamp.o lsampout.wav
