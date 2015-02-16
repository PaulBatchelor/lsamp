#include <stdio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sndfile.h>

#include "lsamp.h"

int main() {
    lsamp_data *ld; 
    lsamp_create_header(&ld);
    lsamp_read_header(ld, "out.smp");
    lsamp_print_header(ld, stdout);
    lsamp_destroy_header(&ld);
    return 0;
}
