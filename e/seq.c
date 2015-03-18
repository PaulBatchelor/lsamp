#include <soundpipe.h>
#include "../lsamp.h"

#define NUMCHAN 4
#define STEPS 16
#define SPEED 5000

#define DOSEQ(NAME,CHAN) if(ud->NAME[ud->step] != -1) { \
    ud->chan[CHAN].reg = ud->NAME[ud->step]; \
    ud->chan[CHAN].pos = 0; \
    lsamp_trig(&ud->chan[CHAN]); \
}

typedef struct {
    uint32_t reg;
    uint32_t pos;
    int32_t bufpos;
    lsamp_cbuf buf;
}lsamp_channel;

typedef struct {
    lsamp_data *ld;
    lsamp_channel chan[NUMCHAN];
    int clock;
    int step;
    int *kick;
    int *snare;
    int *hihat;
}user_data; 

void lsamp_init_channel(lsamp_channel *chan) {
    chan->reg = 0;
    chan->pos = 0;
    /* set to negative two for initialization */
    chan->bufpos = -2;
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
 * on the particular channel 
 *
 * You will need this to trigger and LSAMP instrument at all.
 *
 */
void lsamp_trig(lsamp_channel *chan) {
        chan->bufpos = -1;
}

/* This function was created in order to make buffer usage in lsamp 
 * transparent, which is useful for working with soundpipe.
 * 
 * This function will pop the next value in the circular buffer and return
 * it into the LSAMP_FLOAT pointer out.
 *
 * When the buffer position is full at -1, it will
 * read from the lsamp file and fill the circular buffer. 
 *
 * By default, the buffer position is set to -2, which will return 0;
 */
void lsamp_get(lsamp_data *ld, lsamp_channel *chan, LSAMP_FLOAT *out){
    if(chan->bufpos != -2){
        if(chan->bufpos >= LSAMP_BUFFER_SIZE || chan->bufpos == -1) {
            load_buffer(ld, chan);
        }
        lsamp_cbuf_get_val(&chan->buf, out); 
        chan->bufpos++;
    }else{
        *out = 0;
    }
}

void init(user_data *ud) {
    ud->clock = 0;
    ud->step = -1;
    int i;
    for(i = 0; i < NUMCHAN; i++) { 
        lsamp_init_channel(&ud->chan[i]);
    }
    lsamp_create_header(&ud->ld);
}

void destroy(user_data *ud) {
    lsamp_destroy_header(&ud->ld);
}

void process(sp_data *sp, void *userdata) {
    int i;
    user_data *ud = (user_data *)userdata;
    LSAMP_FLOAT tmp, sum;
    sum = 0;
    if(ud->clock == 0){
        ud->step = (ud->step + 1) % 16;
        DOSEQ(kick, 0);
        DOSEQ(snare, 1);
        DOSEQ(hihat, 2);
    }
    for(i = 0; i < NUMCHAN; i++){
        lsamp_get(ud->ld, &ud->chan[i], &tmp);
        sum += tmp;
    }

    sp->out = sum;
    ud->clock = (ud->clock + 1) % 5000;
}

int main() {
    sp_data *sp;
    user_data ud;
    int k[] = {
        0, -1, -1, -1, 
        0, -1, -1, -1, 
        0, -1, -1, -1, 
        0, -1, -1, -1,
    };
    int s[] = {
        -1, -1, -1, -1, 
        1, -1, 2, -1, 
        -1, -1, -1, -1, 
        1, -1, -1, 2,
    };
    int h[] = {
        -1, -1, 6, -1, 
        -1, -1, 6, -1, 
        -1, -1, 6, -1, 
        -1, -1, 6, -1,
    };

    init(&ud);
    ud.kick = k;
    ud.snare = s;
    ud.hihat= h;
    sp_create(&sp);
    lsamp_read_header(ud.ld, "../smp/tr909.smp");
    sp_process(sp, &ud, process);

    sp_destroy(&sp);
    destroy(&ud);
}
