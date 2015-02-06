#include <sndfile.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "lsamp.h"

void lsamp_add (lsamp_data *ld) {
	char *line = NULL;
	size_t len;
	ssize_t read;

	lsamp_create_header(&ld);
	lsamp_open_tmpfile(ld, "out.bin");

	while(!feof(stdin)) {
		if(line != NULL) {
			printf("%d, %s", read, line);
			lsamp_add_file(ld, line);
		}
		read = getline(&line, &len, stdin);
		if(line[read - 1] == '\n') line[read - 1] = '\0';

	}	

	lsamp_print_header(ld, stdout);
	lsamp_write_header(ld, "out.header");
	free(line);
}

int main(int argc, char **argv) {
	lsamp_data *ld;

	lsamp_add(ld);

	return 0;
}

