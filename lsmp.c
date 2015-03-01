#include <sndfile.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lsamp.h"

#define ARG_ERROR(NUM, USAGE) if(argc < NUM ){ \
    fprintf(stderr, "Usage: " USAGE "\n"); \
    return -1; \
}

#define ARG_MATCH(ARG) if(!strcmp(argv[argpos], ARG))
typedef struct {
    char **argv;
    int argc;
    int *argpos;    
} lsamp_arg_data;

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

void lsamp_info(lsamp_data *ld, lsamp_arg_data *ad) {
    int *argpos = ad->argpos;
    char *smpfile = ad->argv[*ad->argpos];
    *argpos += 1;
    lsamp_create_header(&ld);
    lsamp_read_header(ld, smpfile);
    lsamp_print_header(ld, stdout);
    lsamp_destroy_header(&ld);
}

void lsamp_split(lsamp_data *ld, lsamp_arg_data *ad) {
    int *argpos = ad->argpos;
    int sr = atoi(ad->argv[*ad->argpos]);
    *argpos += 1;
    char *smpfile = ad->argv[*ad->argpos];
    char *outfile;
    printf("the samplerate is %d and the string is %s\n", sr, smpfile);
    lsamp_create_header(&ld);
    lsamp_read_header(ld, smpfile);
    int i;
    for(i = 0; i < ld->num_reg; i++) {
        asprintf(&outfile, "%s.%03d.wav", smpfile, i);
        printf("writing file %s...\n", outfile);
        lsamp_write_sample(ld, smpfile, outfile, i);
        free(outfile);
    }
    lsamp_destroy_header(&ld);
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
        return -1;
    }

    ARG_MATCH("add"){
        ARG_ERROR(4, "lsmp add output FILES");
        argpos++;
        lsamp_add(ld, &ad);
    }else ARG_MATCH("info"){
        ARG_ERROR(3, "lsmp info FILE");
        argpos++;
        lsamp_info(ld, &ad);
    }else ARG_MATCH("split"){
        ARG_ERROR(4, "lsmp split samplerate FILE");
        argpos++;
        lsamp_split(ld, &ad);
    }
	return 0;
}
