#define LSFLOAT float
#define LSBUFSIZE 1024
#define LSAMP_OK 1
#define LSAMP_NOT_OK 0

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sndfile.h>

typedef struct {
    sqlite3 *db;
    SNDFILE *sfile;
    SF_INFO sfinfo;
    LSFLOAT buf[LSBUFSIZE];
} lsamp_data;

typedef struct {
    LSFLOAT *buf;
    sqlite3_blob *blob;
    uint32_t total_frames, frmpos;
}lsamp_handle;

void lsamp_open(lsamp_data **ls, const char *filename);
void lsamp_close(lsamp_data **ls);

void lsamp_create_table(lsamp_data *ls);
void lsamp_add_file(lsamp_data *ls, const char *filename, const char *keyword);
void lsamp_remove_file(lsamp_data *ls, int row_id);

void lsamp_len(lsamp_data *ls, int row_id);

int lsamp_rowid(lsamp_data *ls, const char *keyword);

void lsamp_load_to_buf(lsamp_data *ls, 
        int row_id, 
        int nfloats, 
        int pos, LSFLOAT *buf);


int lsamp_open_sfile(lsamp_data *ls, const char *filename);
int lsamp_close_sfile(lsamp_data *ls);

int lsamp_create_handle(lsamp_handle **lh, uint32_t rowid, uint32_t bufsize);
int lsamp_close_handle(lsamp_handle **lh);
int lsamp_change_sample(lsamp_handle *lh, uint32_t rowid);
int lsamp_load_to_buf(lsamp_handle *lh, uint32_t offset);
