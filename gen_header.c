#include <stdio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "lsamp.h"

int test_1(lsamp_data *ld) {
    /* write some header entries */
    lsamp_create_header(&ld);
    lsamp_add_entry(ld, 12, 34);
    lsamp_add_entry(ld, 56, 78);
    lsamp_add_entry(ld, 910, 1112);
    lsamp_write_header(ld, "out.bin"); 
    lsamp_print_header(ld, stdout);
    lsamp_destroy_header(&ld);
    return 1;
}
int test_2(lsamp_data *ld) {
    /* to be run with test_1 */
    lsamp_create_header(&ld);
    lsamp_read_header(ld, "out.bin");
    lsamp_print_header(ld, stdout);

    printf("Offset of register 1 is %ld\n", lsamp_get_offset(ld, 0));
    printf("Size of register 1 is %ld\n", lsamp_get_size(ld, 0));
    lsamp_destroy_header(&ld);
    return 1;
}
int test_3(lsamp_data *ld) {
    /* Open an audio file */

    lsamp_create_header(&ld);
    lsamp_open_sndfile(ld, "clap.wav");

    printf("the size of file clap.wav is %d\n", lsamp_sndfile_size(ld));

    lsamp_close_sndfile(ld);

    lsamp_destroy_header(&ld);
    return 1;
}
int test_4(lsamp_data *ld) {
    lsamp_create_header(&ld);
	lsamp_open_tmpfile(ld, "out.tmp");
    lsamp_add_file(ld, "clap.wav");
    //lsamp_add_file(ld, "clap.wav");
    lsamp_print_header(ld, stdout);
    /*TODO: valgrind memory error occurs when lsamp_write_header is NOT called...why? */
    lsamp_write_header(ld, "out.bin"); 
    lsamp_destroy_header(&ld);
    return 1;
}
int test_5(lsamp_data *ld) {
/* Open a smp file, extract register 0 to "out.wav" */
	lsamp_create_header(&ld);
	lsamp_read_header(ld, "out.smp");
    printf("Offset of register 1 is %ld\n", lsamp_get_offset(ld, 0));
    printf("Size of register 1 is %ld\n", lsamp_get_size(ld, 0));
	lsamp_write_sample(ld, "out.smp", "lsampout.wav", 0);
	lsamp_destroy_header(&ld);
	return 1;
}
int main() {
    lsamp_data *ld;

    //test_1(ld);
    //test_2(ld);
    //test_3(ld);
    //test_4(ld);
	test_5(ld);
    return 0;
}
