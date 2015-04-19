#include <stdio.h>
#include <string.h>
#include "lsamp.h"

int add_to_lsamp(const char *dbfile, const char *filename, const char *keyword) {
    printf("Adding %s to %s as \"%s\"...\n", filename, dbfile, keyword);
    return 1;
}

int remove_from_lsamp(const char *dbfile, const char *keyword) {
    printf("Removing \"%s\" from %s...\n", keyword, dbfile);
    return 1;
}

int extract_from_lsamp(const char *dbfile, const char *keyword) {
    printf("Extracting \"%s\" from %s...\n", keyword, dbfile);
    return 1;
}

int main(int argc, char *argv[] ) {
    if(argc == 1) {
        error:
        printf("Usage: lsamp [add|remove|extract] lsamp_file keyword (filename)\n");
        return 1;
    }
    if(strncmp(argv[1], "add", 10) && argc >= 5){
        add_to_lsamp(argv[2], argv[3], argv[4]);
    }else if(strncmp(argv[1], "remove", 10) && argc >= 4){
        remove_from_lsamp(argv[2], argv[3]);
    }else if(strncmp(argv[1], "extract", 10) && argc >= 4){
        extract_from_lsamp(argv[2], argv[3]);
    }else{
        /* because I can. */
        goto error;
    }
    return 0;
}
