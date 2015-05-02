#define LSFLOAT float
#define LSBUFSIZE 4096
#define LSAMP_OK 1
#define LSAMP_NOT_OK 0

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sndfile.h>
#include <string.h>

typedef struct {
    sqlite3 *db;
    SNDFILE *sfile;
    SF_INFO sfinfo;
    LSFLOAT buf[LSBUFSIZE];
} lsamp_data;

typedef struct {
    LSFLOAT *buffer;
    sqlite3_blob *blob;
    uint32_t total_frames, frmpos, rowid, bufsize;
}lsamp_handle;

void lsamp_open(lsamp_data **ls, const char *filename);
void lsamp_close(lsamp_data **ls);

void lsamp_create_table(lsamp_data *ls);
void lsamp_add_file(lsamp_data *ls, const char *filename, const char *keyword);
void lsamp_remove_entry(lsamp_data *ls, const char *keyword);
void lsamp_extract_entry(lsamp_data *ls, const char *keyword, const char *outfile);

void lsamp_len(lsamp_data *ls, int row_id);

int lsamp_rowid(lsamp_data *ls, const char *keyword);


int lsamp_open_sfile(lsamp_data *ls, const char *filename);
int lsamp_close_sfile(lsamp_data *ls);

int lsamp_open_handle(lsamp_data *ls, lsamp_handle **lh, uint32_t rowid, uint32_t bufsize);
int lsamp_close_handle(lsamp_handle **lh);
int lsamp_change_sample(lsamp_data *ls, lsamp_handle *lh, uint32_t rowid);
int lsamp_load_to_buf(lsamp_handle *lh, uint32_t offset);

int lsamp_get_bufsize(lsamp_handle *lh);
uint32_t lsamp_get_total_frames(lsamp_handle *lh);
uint32_t lsamp_get_frmpos(lsamp_handle *lh);
void lsamp_set_frmpos(lsamp_handle *lh, uint32_t pos);
LSFLOAT lsamp_get_frame(lsamp_handle *lh, int bufpos);
