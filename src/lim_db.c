#include "lim_db.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cor_core.h"

lim_db_h *
lim_db_new()
{
    lim_db_h *db = (lim_db_h *) malloc(sizeof(lim_db_h));
    if (!db) {
        return NULL;
    }
    memset(db, 0, sizeof(lim_db_h));
    return db;
}

void
lim_db_delete(lim_db_h *db)
{
    if (db) {
        free(db);
    }
}

int64_t
lim_client_limit(lim_db_h *db, unsigned int *keys, int keys_size)
{
    return 0;
}
