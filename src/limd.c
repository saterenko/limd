#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "limd_log.h"


typedef struct
{
    limd_log_t *log;
} limd_ctx_t;

int
main(int argc, char **argv)
{
    limd_ctx_t ctx;
    memset(&ctx, 0, sizeof(limd_ctx_t));
    /**/
    ctx.log = limd_log_new("limd.log", limd_log_level_debug);
    if (!ctx.log) {
        fprintf(stderr, "can't limd_log_new\n");
        return 1;
    }
    /**/
    getchar();
    /**/
    limd_log_delete(ctx.log);
    return 0;
}
