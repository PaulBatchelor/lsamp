#include "lsamp.h"
int make_file() {
    lsamp_data *ls;
    lsamp_open(&ls, "out.smp");
    lsamp_create_table(ls);
    lsamp_add_file(ls, "smp/tr909/01_Snare.wav", "Kick");
    lsamp_close(&ls);
    return 1;
}

int main() {
    make_file();
    return 0;
}
