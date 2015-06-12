#include <stdio.h>
#include <string.h>
#include "lsamp.h"

int add_to_lsamp(const char *dbfile, const char *keyword, const char *filename) {
    printf("Adding %s to %s as \"%s\"...\n", filename, dbfile, keyword);
    lsamp_data *ls;
    lsamp_open(&ls, dbfile);
    lsamp_add_file(ls, filename, keyword);
    lsamp_close(&ls);
    return 1;
}

int init_lsamp_db(const char *dbfile) {
    lsamp_data *ls; 
    lsamp_open(&ls, dbfile);
    lsamp_create_table(ls);
    lsamp_close(&ls);
    return 1;
}

int remove_from_lsamp(const char *dbfile, const char *keyword) {
    printf("Removing \"%s\" from %s...\n", keyword, dbfile);
    lsamp_data *ls;
    lsamp_open(&ls, dbfile);
    lsamp_remove_entry(ls, keyword);
    lsamp_close(&ls);
    return 1;
}

int extract_from_lsamp(const char *dbfile, const char *keyword, const char *outfile) {
    printf("Extracting \"%s\" from %s to %s...\n", keyword, dbfile, outfile);
    lsamp_data *ls;
    lsamp_open(&ls, dbfile);
    lsamp_extract_entry(ls, keyword, outfile);
    lsamp_close(&ls);
    return 1;
}

int main(int argc, char *argv[] ) {
    if(argc == 1) {
        error:
        printf("Usage: lsamp [add|remove|extract|init] lsamp_file keyword (filename)\n");
        return 1;
    }

    if(!strncmp(argv[1], "add", 10) && argc >= 5){
        add_to_lsamp(argv[2], argv[3], argv[4]);
    }else if(!strncmp(argv[1], "init", 10) && argc >= 3){
        init_lsamp_db(argv[2]);
    }else if(!strncmp(argv[1], "remove", 10) && argc >= 4){
        remove_from_lsamp(argv[2], argv[3]);
    }else if(!strncmp(argv[1], "extract", 10) && argc >= 5){
        extract_from_lsamp(argv[2], argv[3], argv[4]);
    }else{
        /* because I can. */
        goto error;
    }
    return 0;
}
