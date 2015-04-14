#include "lsamp.h"

void lsamp_open(lsamp_data **ls, const char *filename) {
    *ls = (lsamp_data *)malloc(sizeof(lsamp_data));
    lsamp_data *lp = *ls;
    sqlite3_open(filename, &lp->db);
}

void lsamp_close(lsamp_data **ls) {
    lsamp_data *lp = *ls;
    sqlite3_close(lp->db);
    free(*ls);
}

void lsamp_create_table(lsamp_data *ls) {
    sqlite3_stmt *ppStmt;
    int rc;
    rc = sqlite3_prepare_v2(ls->db,
            "CREATE TABLE lsamp (keyword STRING UNIQUE, data BLOB);",
            -1,
            &ppStmt,
            NULL);
    /* TODO: Error checking */
    rc = sqlite3_step(ppStmt);
    rc = sqlite3_finalize(ppStmt);

}


int lsamp_open_sfile(lsamp_data *ls, const char *filename) {
    ls->sfile = sf_open(filename, SFM_READ, &ls->sfinfo);

    if(ls->sfile == NULL) {
        fprintf(stderr, "Error: File %s not found.\n", filename);
        return LSAMP_NOT_OK;
    }else if(ls->sfinfo.channels > 1) {
        fprintf(stderr, "Error: Monophonic files only. sound file %s has %d channels.\n",
                filename, ls->sfinfo.channels);
        return LSAMP_NOT_OK;
    }
    return LSAMP_OK;
}

int lsamp_close_sfile(lsamp_data *ls){
    sf_close(ls->sfile);
    return 0;
}

void lsamp_add_file(lsamp_data *ls, const char *filename, const char *keyword) {
    sqlite3_stmt *pStmt;
    int rc;
    int rowid;
    int count;
    int bufsize = LSBUFSIZE;
    int blob_offset = 0;

    if(!lsamp_open_sfile(ls, filename)) {
        return;
    }
    rc = sqlite3_prepare_v2(ls->db,
            "INSERT INTO lsamp(keyword, data) VALUES(?,ZEROBLOB(?));",
            -1,
            &pStmt,
            NULL);
    if(rc) {
        fprintf(stderr, "SQL Error: %s\n", sqlite3_errmsg(ls->db));
        return;
    }
    sqlite3_bind_text(pStmt, 1, keyword, -1, SQLITE_STATIC);
    sqlite3_bind_int(pStmt, 2, ls->sfinfo.frames * sizeof(LSFLOAT));
    rc = sqlite3_step(pStmt);
    rc = sqlite3_finalize(pStmt);
    
    sqlite3_blob *pBlob;
    rc = sqlite3_blob_open(ls->db, "main", "lsamp", "data", 
            sqlite3_last_insert_rowid(ls->db), 1, &pBlob);
    if(rc) {
        fprintf(stderr, "Error opening blob: %s\n", sqlite3_errmsg(ls->db));
        return;
    }
    count = ls->sfinfo.frames;
    while(count != 0) {
#ifdef LS_DEBUG
        fprintf(stderr, "Reading buffersize %d.\n", bufsize);
        fflush(stderr);
#endif
        bufsize = sf_read_float(ls->sfile, ls->buf, bufsize);
        rc = sqlite3_blob_write(pBlob, ls->buf, 
                bufsize * sizeof(LSFLOAT), blob_offset);

#ifdef LS_DEBUG
        fprintf(stderr, "%d bytes Written. Writing %d bytes.\n", 
                blob_offset, bufsize);
        fflush(stderr);
#endif
        if(rc) {
            fprintf(stderr, "SQL Error: %s\n", sqlite3_errmsg(ls->db));
            return;
        }
        blob_offset += bufsize;
        count -= bufsize;
    }
    lsamp_close_sfile(ls);
    sqlite3_blob_close(pBlob);
}

void lsamp_remove_file(lsamp_data *ls, int row_id) {

}

void lsamp_len(lsamp_data *ls, int row_id) {

}

int lsamp_rowid(lsamp_data *ls, const char *keyword) {

}

void lsamp_load_to_buf(lsamp_data *ls, 
        int row_id, 
        int nfloats, 
        int pos, LSFLOAT *buf) {
}
