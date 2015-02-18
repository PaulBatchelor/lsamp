#include <sndfile.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lsamp.h"

typedef struct {
    char **argv;
    int argc;
    int *argpos;    
} lsamp_arg_data;

void lsamp_combine(char *header, char *data) {
    printf("combining %s and %s...\n", header, data);
    FILE *hp = fopen(header, "a");
    FILE *dp = fopen(data, "r");
    char c;
    while(1){
        c = fgetc(dp);
        if(c == EOF) break;
        fputc(c, hp);
    }
    fclose(dp);
    fclose(hp);
    unlink(data);
}

void lsamp_add(lsamp_data *ld, lsamp_arg_data *ad) {
    int i;
    int *argpos = ad->argpos;
    char *smpfile = ad->argv[*ad->argpos];
    char *tmpfile;
    asprintf(&tmpfile, "%s.tmp", smpfile);
    printf("the tmp file is %s\n", tmpfile);
    *argpos += 1;
    printf("Writing to file %s\n", smpfile);

    lsamp_create_header(&ld);
    lsamp_open_tmpfile(ld, tmpfile);
    for(i = *argpos; i < ad->argc; i++){
        printf("adding %s...\n", ad->argv[i]);
        lsamp_add_file(ld, ad->argv[i]);
    }
    lsamp_write_header(ld, smpfile);
    lsamp_destroy_header(&ld);
    lsamp_combine(smpfile, tmpfile);
    free(tmpfile);
}

int main(int argc, char **argv) {
	lsamp_data *ld;
    lsamp_arg_data ad; 
    int argpos = 1;
    ad.argc = argc;
    ad.argv = argv;
    ad.argpos = &argpos;
    if(argc < 2) {
        fprintf(stderr, "Not enough args\n");
    }
    if(!strcmp(argv[argpos], "add")){
        if(argc  < 4 ){
            fprintf(stderr, "Usage: lsmp add output FILES\n");
            return -1;
        }
        printf("Adding samples...\n");
        argpos++;
        lsamp_add(ld, &ad);
    }
	return 0;
}

