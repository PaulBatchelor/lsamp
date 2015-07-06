#include "lsamp.h"

int lsamp_open(lsamp_data **ls, const char *filename, int mode) {
     if(mode == LSAMP_READ && access(filename, F_OK)) {
         fprintf(stderr, "Error: Could not open file %s\n", filename);
         return 0;
     }

    *ls = (lsamp_data *)malloc(sizeof(lsamp_data));
    lsamp_data *lp = *ls;
    sqlite3_open(filename, &lp->db);
    return 1;
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
    ls->sfile = sf_open(filename, SFM_READ , &ls->sfinfo);

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
                bufsize * sizeof(LSFLOAT), blob_offset * sizeof(LSFLOAT));

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

void lsamp_remove_entry(lsamp_data *ls, const char *keyword) {
    //fprintf(stderr, "This feature has not been implemented yet. Sorry about that\n");
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(ls->db,
            "delete from lsamp where keyword=?;",
            -1,
            &stmt,
            NULL);
    if(rc) {
        fprintf(stderr, "Remove entry error: %s\n", sqlite3_errmsg(ls->db));
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_bind_text(stmt, 1, keyword, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void lsamp_extract_entry(lsamp_data *ls, const char *keyword, const char *outfile) {
    fprintf(stderr, "This feature has not been implemented yet. Sorry about that\n");
}
void lsamp_len(lsamp_data *ls, int row_id) {

}

int lsamp_rowid(lsamp_data *ls, const char *keyword) {
    uint32_t rowid;
    int rc;
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(ls->db,
            "SELECT rowid FROM lsamp WHERE keyword = ?;",
            -1,
            &stmt,
            NULL);

    if(rc) {
        fprintf(stderr, "SQL Error: %s\n", sqlite3_errmsg(ls->db));
        return;
    }
    sqlite3_bind_text(stmt, 1, keyword, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    rowid = sqlite3_column_int(stmt, 0);
#ifdef LS_DEBUG
        fprintf(stderr, "The rowid is %d.\n", rowid);
        fflush(stderr);
#endif
    sqlite3_finalize(stmt);
    return rowid;
}

void lsamp_zero_handle(lsamp_handle *lh) {
    int i;
    for(i = 0; i < lh->bufsize; i++) {
        lh->buffer[i] = 0;
    }
}

int lsamp_extract_info(lsamp_data *ls, lsamp_handle *lh, uint32_t rowid) {
    /* is this function needed, or are we okay with memset? 
    lsamp_zero_handle(lp);
    */
    lh->rowid = rowid;
    lh->total_frames = 0;
    lh->frmpos = 0; 

    int rc = sqlite3_blob_open(ls->db, "main", "lsamp", "data", 
            rowid, 0, &lh->blob);
    if(rc) {
        fprintf(stderr, "lsamp_open_handle() Error: %s\n", sqlite3_errmsg(ls->db));
        return 0;
    }
    lh->total_frames = sqlite3_blob_bytes(lh->blob) / sizeof(LSFLOAT);
#ifdef LS_DEBUG
        fprintf(stderr, "lsamp_open_handle: lh->total_frames is %d.\n", lh->total_frames);
        fflush(stderr);
#endif
    return 1;
}

int lsamp_open_handle(lsamp_data *ls, lsamp_handle **lh, 
        uint32_t rowid, uint32_t bufsize) {
    int rc;
    *lh = (lsamp_handle *)malloc(sizeof(lsamp_handle));
    lsamp_handle *lp = *lh;
    lp->bufsize = bufsize;
    lp->buffer = (LSFLOAT *)malloc(bufsize * sizeof(LSFLOAT));
    if(lsamp_extract_info(ls, *lh, rowid)){
        return LSAMP_OK;
    }else{
        return LSAMP_NOT_OK;
    }
}
int lsamp_close_handle(lsamp_handle **lh){
    lsamp_handle *lp = *lh;
    sqlite3_blob_close(lp->blob);
    free(lp->buffer);
    free(lp);
}
int lsamp_change_sample(lsamp_data *ls, lsamp_handle *lh, uint32_t rowid){
    if(lsamp_extract_info(ls, lh, rowid)){
        return LSAMP_OK;
    }else{
        return LSAMP_NOT_OK;
    }
}
int lsamp_load_to_buf(lsamp_handle *lh, uint32_t offset){
    int bufsize = 0;
    int rc;
    memset(lh->buffer, 0, lh->bufsize);
    if(lh->total_frames - lh->frmpos >= lh->bufsize) {
        bufsize = lh->bufsize; 
    }else{
#ifdef LS_DEBUG
        printf("lsamp_load_to_buf: using smaller buffer size.\n");
        //fflush(stderr);
#endif
        bufsize = lh->total_frames - lh->frmpos;
    }
    rc = sqlite3_blob_read(lh->blob, lh->buffer, bufsize * sizeof(LSFLOAT), offset * sizeof(LSFLOAT));
    if(rc) {
        //fprintf(stderr, "lsamp_load_to_buf() Error\n");
        printf("lsamp_load_to_buf() Error\n");
        return 0;
    }
    lh->frmpos += bufsize;
    return 1;
}
int lsamp_get_bufsize(lsamp_handle *lh) {
    return lh->bufsize;
}

LSFLOAT lsamp_get_frame(lsamp_handle *lh, int bufpos) {
    if(bufpos > lh->bufsize) {
        return 0;
    }else{
        return lh->buffer[bufpos];
    }
}
uint32_t lsamp_get_frmpos(lsamp_handle *lh) {
    return lh->frmpos;
}
void lsamp_set_frmpos(lsamp_handle *lh, uint32_t pos) {
    lh->frmpos = pos;
}
uint32_t lsamp_get_total_frames(lsamp_handle *lh) {
    return lh->total_frames;
}
