#include <stdio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sndfile.h>

#include "lsamp.h"

/* Create / Destroy */

void lsamp_create_header(lsamp_data **ld) {
    *ld = malloc(sizeof(lsamp_data));
    lsamp_data *lp = *ld;
    
    lp->header_size = 2 * sizeof(uint32_t) + sizeof(uint16_t); //+ num_reg * sizeof(lsamp_entry);
    lp->data_size= 0;
    lp->pos = 0;
    lp->num_reg= 0;
    lp->mode = LSAMP_INIT;
}

void lsamp_destroy_header(lsamp_data **ld) {
    if((*ld)->mode != LSAMP_INIT) {
        free((*ld)->entry);
        fclose((*ld)->fp);
    }
    free(*ld);
}


/* DB Writing Operations */

void lsamp_write_header(lsamp_data *ld, const char *fname) {
    int i;

    /*TODO: Make this error free memory properly */
    if(ld->mode != LSAMP_INIT) {
        fprintf(stderr, "ERROR: mode already set");
        exit(0);
    }    

    ld->fp = fopen(fname, "wb");
    
    LSWRITE_32(header_size);
    LSWRITE_32(data_size);
    LSWRITE_16(num_reg);
    for(i = 0; i < ld->num_reg; i++) {
        LSWRITE_32(entry[i].offset);
        LSWRITE_32(entry[i].size);
    }
    ld->mode = LSAMP_WRITE;
}

void lsamp_add_entry(lsamp_data *ld, uint32_t offset, uint32_t size) {
    if(ld->num_reg == 0) {
        ld->entry = malloc(sizeof(lsamp_entry));
    }else if(ld->num_reg > 0) {
        ld->entry = (lsamp_entry *)realloc(ld->entry, 
                (ld->num_reg + 1) * sizeof(lsamp_entry));
    }
    ld->entry[ld->num_reg].offset = offset;
    ld->entry[ld->num_reg].size = size;
    ld->num_reg++;
    ld->header_size += sizeof(lsamp_entry);
}

/* DB Reading Operations */

void lsamp_read_header(lsamp_data *ld, const char *fname) {
    int i;
    ld->fp = fopen(fname, "rb");
    LSREAD_32(header_size);
    LSREAD_32(data_size);
    LSREAD_16(num_reg);
    ld->entry = malloc( ld->num_reg * sizeof(lsamp_entry) );
    printf("the size is %d\n", ld->num_reg);
    for(i = 0; i < ld->num_reg; i++) {
        LSREAD_32(entry[i].offset);
        LSREAD_32(entry[i].size);
    }

    ld->mode = LSAMP_READ;
}

void lsamp_print_header(lsamp_data *ld, FILE *fp) {
    fprintf(stdout, "The header size is %d\n", ld->header_size);
    fprintf(stdout, "The data size is %d\n", ld->data_size);
    fprintf(stdout, "There are %d registers\n", ld->num_reg);
    int i;
    fprintf(stdout, "Registers:\n", ld->num_reg);
    for(i = 0; i < ld->num_reg; i++) {
    fprintf(stdout, "%d: %d offset %d size\n", 
            i, ld->entry[i].offset, ld->entry[i].size);
    }
}

uint32_t lsamp_get_size(lsamp_data *ld, uint32_t reg) {
    uint32_t size;
    uint32_t *ptr = &size;
    if( reg > (ld->num_reg - 1) ) {
        fprintf(stderr, "ERROR: register %d is out of bounds (%d registers)\n",
                reg, ld->num_reg);
        exit(0);
    }
    fseek(ld->fp, 
            3 * sizeof(uint32_t) + sizeof(uint16_t) + reg * 2 * sizeof(uint32_t), SEEK_SET);
    fread(ptr, sizeof(uint32_t), 1, ld->fp);
    return size;
}

uint32_t lsamp_get_offset(lsamp_data *ld, uint32_t reg) {
    uint32_t off;
    uint32_t *ptr = &off;
    if( reg > (ld->num_reg - 1) ) {
        fprintf(stderr, "ERROR: register %d is out of bounds (%d registers)\n",
                reg, ld->num_reg);
        exit(0);
    }
    fseek(ld->fp, 
            2 * sizeof(uint32_t) + sizeof(uint16_t) + reg * 2 * sizeof(uint32_t), SEEK_SET);
    fread(ptr, sizeof(uint32_t), 1, ld->fp);
    return off;
}

/* File Operations */

void lsamp_open_sndfile(lsamp_data *ld, const char *fname) {
    ld->sndfile.file = sf_open(fname, SFM_READ, &ld->sndfile.info);
}

uint32_t lsamp_sndfile_size(lsamp_data *ld) {
    return ld->sndfile.info.frames;
}

void lsamp_close_sndfile(lsamp_data *ld) {
    sf_close(ld->sndfile.file);
}
void lsamp_write_sndfile(lsamp_data *ld) {

}
void lsamp_add_file(lsamp_data *ld, const char *fname) {
    lsamp_open_sndfile(ld, fname);
    lsamp_add_entry(ld, ld->data_size, lsamp_sndfile_size(ld));
    lsamp_write_sndfile(ld);
    ld->data_size += lsamp_sndfile_size(ld);
    lsamp_close_sndfile(ld);
}