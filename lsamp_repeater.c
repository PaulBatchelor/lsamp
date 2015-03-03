#include <stdio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sndfile.h>
#include <string.h>

#include "lsamp.h"

#define BUFLEN 1024
#define LSBUFSIZE 1024

typedef struct {
    lsamp_data *ld;
    uint32_t reg;
    long length;
    uint32_t numsamps;
    uint32_t pos;
    uint32_t ppos;
    long clock;
    uint32_t bufpos;
    LSAMP_FLOAT tmp[LSBUFSIZE];
    lsamp_cbuf buf;
} myData;

void load_buffer(myData *data) {
    int i;
    data->pos += lsamp_read_to_buf(data->ld, data->tmp, 
            LSBUFSIZE, data->reg, data->pos);
    for(i = 0; i < LSBUFSIZE; i++){
        lsamp_cbuf_put_val(&data->buf, data->tmp[i]);
    }
   //data->pos += lsamp_read_to_cbuf(data->ld, &data->buf, data->reg, data->pos);
}

void get_bufval(LSAMP_FLOAT *buf, myData *data) {
    lsamp_cbuf_get_val(&data->buf, buf);
}

void process(LSAMP_FLOAT *buf, myData *data) {
    if(data->bufpos >= LSBUFSIZE || data->pos == 0) {
        load_buffer(data);
        data->bufpos = 0;
    }
    if(data->clock == 20000) {
        data->clock = 0;
        data->pos = 0;
        if(data->reg == 0){
            data->reg = 1;
        }else{
            data->reg = 0;
        }
    }
    data->bufpos++;
    get_bufval(buf, data);
    data->clock++;
}

int main() {
    myData data;
    lsamp_create_header(&data.ld);
    lsamp_read_header(data.ld, "smp/tr909.smp");
    lsamp_print_header(data.ld, stdout);
    data.length = 3 * 44100;
    data.reg = 0;
    data.pos = 0;
    data.clock = 0;
    data.bufpos = 0;
    lsamp_cbuf_init(&data.buf);
    printf("writing silence to disk \n");
    SNDFILE *file;
    SF_INFO info;
    info.frames = data.length;
    info.samplerate = 44100;
    info.channels = 1;
    info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;   
    LSAMP_FLOAT buffer[BUFLEN];
    uint32_t i; 
    file = sf_open("silence.wav", SFM_WRITE, &info); 
    while(data.length != 0) {
        if(data.length < BUFLEN) {
            data.numsamps = data.length;
        }else{
            data.numsamps = BUFLEN;
        }
        for(i = 0; i < BUFLEN; i++) {
            process(&buffer[i], &data);

        }
        sf_write_float(file, buffer, data.numsamps);
        data.length -= data.numsamps;
    }

    sf_close(file);
    lsamp_destroy_header(&data.ld);
    return 0;
}
