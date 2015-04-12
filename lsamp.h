#define LSFLOAT float
typedef struct {


} LSAMP_DATA;

void lsamp_open(LSAMP_DATA *ls);
void lsamp_close(LSAMP_DATA *ls);

void lsamp_create_table(LSAMP_DATA *ls);
void lsamp_add_file(LSAMP_DATA *ls, char *filename, const char *keyword);
void lsamp_remove_file(LSAMP_DATA *ls, int row_id);

void lsamp_len(LSAMP_DATA *ls, int row_id);

int lsamp_rowid(LSAMP_DATA *ls, const char *keyword);

void lsamp_load_to_buf(LSAMP_DATA *ls, 
        int row_id, 
        int nfloats, 
        int pos, LSFLOAT *buf);
