#ifndef LIM_DB_H
#define LIM_DB_H

#include <inttypes.h>

typedef struct lim_db_el_s lim_db_el_t;

struct lim_db_el_s {
    uint64_t key;
    int64_t limit;
    int64_t spent;
    int64_t locked;
    lim_db_el_t *els;
};

typedef struct {
    lim_db_el_s *els;
    lim_db_el_s *els_free;
} lim_db_h;

lim_db_h *lim_db_new();
void lim_db_delete(lim_db_h *db);
int64_t lim_client_limit(lim_db_h *db, unsigned int *keys, int keys_size);

#endif
