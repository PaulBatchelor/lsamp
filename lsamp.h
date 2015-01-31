#define LSWRITE_32(val) fwrite(&ld->val, sizeof(uint32_t), 1, ld->fp)
#define LSWRITE_16(val) fwrite(&ld->val, sizeof(uint16_t), 1, ld->fp)

#define LSREAD_32(val) fread(&ld->val, sizeof(uint32_t), 1, ld->fp)
#define LSREAD_16(val) fread(&ld->val, sizeof(uint16_t), 1, ld->fp)

enum {
    LSAMP_INIT,
    LSAMP_READ,
    LSAMP_WRITE
};

typedef FILE lsamp_fp_data;
typedef FILE lsamp_fp_out;

typedef struct {
    uint32_t offset;
    uint32_t size;
}lsamp_entry;

typedef struct {
    SNDFILE *file;
    SF_INFO info;    
} lsamp_sndfile;

typedef struct {
    uint32_t header_size;
    uint32_t data_size;
    uint16_t num_reg;
    uint16_t pos;
    lsamp_fp_out *fp;
    lsamp_fp_data *data;
    lsamp_entry *entry;
    char mode;
    lsamp_sndfile sndfile;
}lsamp_data;

typedef struct {
    int32_t *buf;
    int max_size;
    int size;
}lsamp_buffer;

/* Create / Destroy */

void lsamp_create_header(lsamp_data **ld);
void lsamp_destroy_header(lsamp_data **ld);

/* DB Writing Operations */

void lsamp_write_header(lsamp_data *ld, const char *fname);
void lsamp_add_entry(lsamp_data *ld, uint32_t offset, uint32_t size);

/* DB Reading Operations */

void lsamp_read_header(lsamp_data *ld, const char *fname);
void lsamp_print_header(lsamp_data *ld, FILE *fp);
uint32_t lsamp_get_size(lsamp_data *ld, uint32_t reg);
uint32_t lsamp_get_offset(lsamp_data *ld, uint32_t reg);

/* File Operations */

void lsamp_open_sndfile(lsamp_data *ld, const char *fname);
void lsamp_close_sndfile(lsamp_data *ld);
uint32_t lsamp_sndfile_size(lsamp_data *ld);
void lsamp_write_sndfile(lsamp_data *ld);
void lsamp_add_file(lsamp_data *ld, const char *fname);
