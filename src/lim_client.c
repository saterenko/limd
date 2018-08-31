#include "lim_db.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cor_core.h"

lim_client_h *
lim_client_new()
{
    lim_client_h *client = (lim_client_h *) malloc(sizeof(lim_client_h));
    if (!client) {
        return NULL;
    }
    memset(client, 0, sizeof(lim_client_h));
    /**/
    client->db = lim_db_new();
    if (!client->db) {
        lim_client_delete(client);
        return NULL;
    }
    /**/
    return client;
}

void
lim_client_delete(lim_client_h *client)
{
    if (client) {
        if (client->db) {
            lim_db_delete(client->db);
        }
        free(client);
    }
}

int64_t
lim_client_limit(lim_client_h *client, unsigned int *keys, int keys_size)
{
    return lim_db_limit(client->db, keys, key_size);
}

int
lim_client_limit_set(lim_client_h *client, unsigned int *keys, int keys_size, int64_t value)
{
    return cor_ok;
}

int64_t
lim_client_spent(lim_client_h *client, unsigned int *keys, int keys_size)
{
    return 0;
}

int
lim_client_spent_set(lim_client_h *client, unsigned int *keys, int keys_size, int64_t value)
{
    return cor_ok;
}

int
lim_client_spent_add(lim_client_h *client, unsigned int *keys, int keys_size, int64_t value)
{
    return cor_ok;
}

uint64_t
lim_client_spent_lock(lim_client_h *client, unsigned int *keys, int keys_size, int64_t value)
{
    return 0;
}

int
lim_client_spent_commit(lim_client_h *client, uint64_t id)
{
    return cor_ok;
}

int
lim_client_spent_rollback(lim_client_h *client, uint64_t id)
{
    return cor_ok;
}

int
lim_client_spent_update(lim_client_h *client, uint64_t id, int64_t value)
{
    return cor_ok;
}
