#include <soundpipe.h>
#include "../lsamp.h"

typedef struct {
    uint32_t reg;
    uint32_t pos;
    uint32_t bufpos;
    lsamp_cbuf buf;
}lsamp_channel;

typedef struct {
    lsamp_data *ld;
    lsamp_channel chan[4];
    int clock;
}user_data; 

void lsamp_init_channel(lsamp_channel *chan) {
    chan->reg = 0;
    chan->pos = 0;
    /* set to negative one for initalization */
    chan->bufpos = -1;
    lsamp_cbuf_init(&chan->buf);
}

void load_buffer(lsamp_data *ld, lsamp_channel *chan){
    int i;
    LSAMP_TMP(tmp);
    chan->pos += lsamp_read_to_buf(ld, tmp, 
            LSAMP_BUFFER_SIZE, chan->reg, chan->pos);
    for(i = 0; i < LSAMP_BUFFER_SIZE; i++){
        lsamp_cbuf_put_val(&chan->buf, tmp[i]);
    }
    chan->bufpos = 0;
}

/* this will force a reload of the internal buffer when lsamp_get is called 
 * again on the particular channel 
 *
 * This is needed for changing between samples on a single channel.
 *
 */
void lsamp_retrig(lsamp_channel *chan) {
        chan->bufpos = -1;
}

/* This function was created in order to make buffer usage in lsamp 
 * transparent, which is useful for working with soundpipe.
 * 
 * This function will pop the next value in the circular buffer and return
 * it into the LSAMP_FLOAT pointer out.
 *
 * When the buffer position is full or initalized (-1), it will
 * read from the lsamp file and fill the circular buffer. 
 */
void lsamp_get(lsamp_data *ld, lsamp_channel *chan, LSAMP_FLOAT *out){

    if(chan->bufpos >= LSAMP_BUFFER_SIZE || chan->bufpos == -1) {
        load_buffer(ld, chan);
    }
    lsamp_cbuf_get_val(&chan->buf, out); 
    chan->bufpos++;
}

void init(user_data *ud) {
    ud->clock = 0;
    lsamp_init_channel(&ud->chan[0]);
    lsamp_create_header(&ud->ld);
}

void destroy(user_data *ud) {
    lsamp_destroy_header(&ud->ld);
}

void process(sp_data *sp, void *userdata) {
    user_data *ud = (user_data *)userdata;
    if(ud->clock == 5000){
        ud->chan[0].pos = 0;
        ud->clock = 0;
        if(ud->chan[0].reg == 0){
            ud->chan[0].reg = 1;
        }else{
            ud->chan[0].reg = 0;
        }
        lsamp_retrig(&ud->chan[0]);
    }
    ud->clock++;
    lsamp_get(ud->ld, &ud->chan[0], &sp->out);
}

int main() {
    sp_data *sp;
    user_data ud;
    init(&ud);
    sp_create(&sp);

    lsamp_read_header(ud.ld, "../smp/tr909.smp");
    sp_process(sp, &ud, process);

    sp_destroy(&sp);
    destroy(&ud);
}
