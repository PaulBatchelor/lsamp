#include <sndfile.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lsamp.h"

int main() {
char header[] = "combine_test.smp";
char data[] = "combine_test.smp.tmp"; 

lsamp_combine(header, data);

return 0;
}
