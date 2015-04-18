#include "lsamp.h"
int make_file() {
    lsamp_data *ls;
    lsamp_open(&ls, "out.smp");
    lsamp_create_table(ls);
    lsamp_add_file(ls, "oneart.wav", "madison");
    lsamp_add_file(ls, "01_Snare.wav", "Snare");
    lsamp_close(&ls);
    return 1;
}

int write_raw_file() {
    lsamp_data *ls;
    lsamp_handle *lh;
    int count = 0, bufsize = 1024;
    LSFLOAT zero = 0;
    FILE *out = fopen("out.raw", "w");;

    lsamp_open(&ls, "out.smp");
    lsamp_open_handle(ls, &lh, lsamp_rowid(ls, "madison"), bufsize);
    count = lh->total_frames;
    while(count > 0) {
        if(lh->total_frames - lh->frmpos < bufsize) {
            bufsize = lh->total_frames - lh->frmpos;
        }
        lsamp_load_to_buf(lh, lh->frmpos);
        fwrite(lh->buffer, sizeof(LSFLOAT), bufsize, out);
        count -= bufsize;
    }

    lsamp_close_handle(&lh);
    lsamp_close(&ls);
    fclose(out);
    return 1;
}

int main() {
    make_file();
    write_raw_file();
    return 0;
}
