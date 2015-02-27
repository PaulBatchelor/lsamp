#include <stdio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sndfile.h>
#include <string.h>

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
	lp->buf_open = 0;

}
void lsamp_destroy_header(lsamp_data **ld) {
    if((*ld)->mode != LSAMP_INIT) {
        free((*ld)->entry);
        fclose((*ld)->fp);
    }
	if((*ld)->buf_open) {
		fclose((*ld)->data);
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
	if(ld->buf_open) {
		/* this is where we will concatenate the two files */
	}
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
    for(i = 0; i < ld->num_reg; i++) {
        LSREAD_32(entry[i].offset);
        LSREAD_32(entry[i].size);
    }

    ld->mode = LSAMP_READ;
}

void lsamp_print_header(lsamp_data *ld, FILE *output) {
    fprintf(output, "The header size is %d\n", ld->header_size);
    fprintf(output, "The data size is %d\n", ld->data_size);
    fprintf(output, "There are %d registers\n", ld->num_reg);
    int i;
    fprintf(output, "Registers:\n", ld->num_reg);
    for(i = 0; i < ld->num_reg; i++) {
    fprintf(output, "%d: %d offset %d size\n", 
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
    } fseek(ld->fp, 
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
	if(!ld->buf_open) {
		fprintf(stderr, "ERROR: TMP sndfile is not openend.\n");
		exit(0);
	}
	while((ld->buf.count = sf_read_float(ld->sndfile.file, ld->buf.buf, LSAMP_BUFFER_SIZE))) {
		fwrite(ld->buf.buf, sizeof(LSAMP_FLOAT), ld->buf.count, ld->data);
	}
}

void lsamp_add_file(lsamp_data *ld, const char *fname) {
    lsamp_open_sndfile(ld, fname);
    lsamp_add_entry(ld, ld->data_size, lsamp_sndfile_size(ld) * sizeof(LSAMP_FLOAT));
    lsamp_write_sndfile(ld);
    ld->data_size += lsamp_sndfile_size(ld) * sizeof(LSAMP_FLOAT);
    lsamp_close_sndfile(ld);
}

void lsamp_open_tmpfile(lsamp_data *ld, const char *tmpfile) {
	if(ld->buf_open) {
		fprintf(stderr, "ERROR: buffer is already opened.");
		exit(0);
	}
	ld->data = fopen(tmpfile, "wb");
	ld->buf_open = 1;
}
void lsamp_write_sample(lsamp_data *ld, const char *lsmpfile, const char *outfile, uint32_t pos) {
	SF_INFO info;
	SNDFILE *wavfile;
	uint32_t file_size = lsamp_get_size(ld, pos) / sizeof(LSAMP_FLOAT);
//	uint32_t file_size = LSAMP_BUFFER_SIZE;
	uint32_t samples_left = file_size;
	uint32_t bufsize = LSAMP_BUFFER_SIZE;
//	long file_offset = ld->header_size + (lsamp_get_offset(ld, pos) * sizeof(LSAMP_FLOAT));
	long file_offset = ld->header_size + lsamp_get_offset(ld, pos);
	info.samplerate = 44100;
	info.channels = 1;
	info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24; 

	wavfile = sf_open(outfile, SFM_WRITE, &info);
	printf("the file offset is %ld\n", file_offset);
	fseek(ld->fp, file_offset, SEEK_SET);
	printf("the filesize is %ld. and %ld samples left\n", file_size, samples_left);
	while(samples_left > 0) {
		if(samples_left < LSAMP_BUFFER_SIZE) {
			bufsize = samples_left;
		}
		fread(ld->buf.buf, sizeof(LSAMP_FLOAT), bufsize, ld->fp);
		samples_left -= sf_writef_float(wavfile, ld->buf.buf, bufsize);
	}

	sf_close(wavfile);
}

/* This function will read data_size number of LSAMP_FLOATs into data 
 * and return  the number of samples read. 
 *
 * If the sample is done being read, we need to do something about it, which is
 * to return 0's.
 *
 * An explantion for how this all works: 
 *
 * sample size (we know this)
 * |--------------------|
 * buffers (we know this)
 * |-----|-----|-----|-----|
 * samples needed (we don't know this)
 *                   |--|
 * 
 * the lsamp size is expressed in bytes, not samples. so the sample size is
 * divided by sizeof(LSAMP_FLOAT)
 *
 * unused_part_of_buffer = (pos + buffer_size) - sample_size
 * used_part_of_buffer = buffer_size - unused_part_of_buffer 
 * therefore...
 * samplesread = buffer_size - ((pos + buffer_size) - sample_size)
 *
 * TODO: 
 *  - rename data_size to buffer_size
 *  - move this comment somewhere else?
 * 
 */
uint32_t lsamp_read_to_buf(lsamp_data *ld, LSAMP_FLOAT *data, 
    uint32_t data_size, uint32_t reg, uint32_t pos){
    uint32_t offset = lsamp_get_offset(ld, reg);
    uint32_t sample_size = lsamp_get_size(ld, reg) / sizeof(LSAMP_FLOAT);
    uint32_t sampsread = data_size;
    memset(data, 0, data_size * sizeof(LSAMP_FLOAT)); 
    if(pos + data_size > sample_size) {
        sampsread = data_size - ((pos + data_size) - sample_size);
    }
    fseek(ld->fp, ld->header_size + offset + pos * sizeof(LSAMP_FLOAT), SEEK_SET);
    fread(data, sizeof(LSAMP_FLOAT), sampsread, ld->fp);
    return sampsread;
}

void lsamp_combine(char *header, char *data) {
    printf("combining %s and %s...\n", header, data);
    FILE *hp, *dp;
    hp = fopen(header, "a");
    if(hp == NULL){
        printf("Error: Could not open file %s\n", header);
        return;
    }

    dp = fopen(data, "rb");
    if(dp == NULL){
        printf("Error: Could not open file %s\n", data);
        return;
    }
    int c;
    int count = 0;
    while((c = getc(dp)) != EOF) {
        fputc(c, hp);
    }
    fclose(dp);
    fclose(hp);
    unlink(data);
}
