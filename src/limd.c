#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "cor_log.h"


typedef struct
{
    cor_log_t *log;
} lim_ctx_t;

int
main(int argc, char **argv)
{
    lim_ctx_t ctx;
    memset(&ctx, 0, sizeof(lim_ctx_t));
    /**/
    ctx.log = cor_log_new("limd.log", cor_log_level_debug);
    if (!ctx.log) {
        fprintf(stderr, "can't cor_log_new\n");
        return 1;
    }
    /**/
    getchar();
    /**/
    cor_log_delete(ctx.log);
    return 0;
}
