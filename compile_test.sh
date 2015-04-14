gcc -DLS_DEBUG -c lsamp.c
gcc -lsndfile -lsqlite3 t/create_lsamp_file.c -I./ lsamp.o -o create_lsamp_file
