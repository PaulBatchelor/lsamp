#include <stdio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sndfile.h>
#include <string.h>

#include "lsamp.h"

#define BUFLEN 1024

typedef struct {
    lsamp_data *ld;
    uint32_t reg;
    long length;
    uint32_t numsamps;
    uint32_t pos;
} myData;

void process(LSAMP_FLOAT *buf, int len, myData *data) {
    memset(buf, 0, len * sizeof(LSAMP_FLOAT)); 
    lsamp_read_to_buf(data->ld, buf, len, data->reg, data->pos);
}

int main() {
    myData data;
    lsamp_create_header(&data.ld);
    lsamp_read_header(data.ld, "out.smp");
    lsamp_print_header(data.ld, stdout);
    data.length = 44100;
    data.reg = 0;
    data.pos = 0;
    printf("writing silence to disk \n");
    SNDFILE *file;
    SF_INFO info;
    info.frames = data.length;
    info.samplerate = 44100;
    info.channels = 1;
    info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;   
    LSAMP_FLOAT buffer[BUFLEN];

    file = sf_open("silence.wav", SFM_WRITE, &info); 
    while(data.length != 0) {
        if(data.length < BUFLEN) {
            data.numsamps = data.length;
        }else{
            data.numsamps = BUFLEN;
        }
        process(buffer, data.numsamps, &data);
        sf_writef_float(file, buffer, data.numsamps);
        data.length -= data.numsamps;
    }

    sf_close(file);
    lsamp_destroy_header(&data.ld);
    return 0;
}
