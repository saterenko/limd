#ifndef LIM_CLIENT_H
#define LIM_CLIENT_H

#include <inttypes.h>

#include "lim_db.h"

typedef struct {
    lim_db_t *db;
} lim_client_h;

lim_client_h *lim_client_new();
void lim_client_delete(lim_client_h *client);

int64_t lim_client_limit(lim_client_h *client, unsigned int *keys, int keys_size);
int lim_client_limit_set(lim_client_h *client, unsigned int *keys, int keys_size, int64_t value);
int64_t lim_client_spent(lim_client_h *client, unsigned int *keys, int keys_size);
int lim_client_spent_set(lim_client_h *client, unsigned int *keys, int keys_size, int64_t value);
int lim_client_spent_add(lim_client_h *client, unsigned int *keys, int keys_size, int64_t value);
uint64_t lim_client_spent_lock(lim_client_h *client, unsigned int *keys, int keys_size, int64_t value);
int lim_client_spent_commit(lim_client_h *client, uint64_t id);
int lim_client_spent_rollback(lim_client_h *client, uint64_t id);
int lim_client_spent_update(lim_client_h *client, uint64_t id, int64_t value);


// int lim_client_spent(lim_client_h *client, unsigned int *keys, int keys_size, int64_t value);

#endif
