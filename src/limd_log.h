#ifndef LIMD_LOG_H
#define LIMD_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "limd_core.h"

enum limd_log_level_e
{
    limd_log_level_error = 0,
    limd_log_level_warn,
    limd_log_level_info,
    limd_log_level_debug
};

#define LIMD_LOG_TIME_STR_SIZE sizeof("[YYYY.MM.DD HH:MM:SS] ")
#define LIMD_LOG_PID_STR_SIZE sizeof("-18446744073709551615")
#define LIMD_LOG_STR_SIZE 1024
#define LIMD_LOG_STR_MAX_SIZE 64 * 1024

#define limd_log_error(log, args...) limd_log_put(log, limd_log_level_error, __FILE__, __LINE__, args)
#define limd_log_warn(log, args...) limd_log_put(log, limd_log_level_warn, __FILE__, __LINE__, args)
#define limd_log_info(log, args...) limd_log_put(log, limd_log_level_info, __FILE__, __LINE__, args)
#define limd_log_debug(log, args...) limd_log_put(log, limd_log_level_debug, __FILE__, __LINE__, args)

typedef struct
{
    int level;
    time_t ts;
    size_t max_line_size;
    char ts_str[LIMD_LOG_TIME_STR_SIZE];
    char pid_str[LIMD_LOG_PID_STR_SIZE];
    FILE *fd;
} limd_log_t;

limd_log_t *limd_log_new(const char *file, int level);
void limd_log_set_pid(limd_log_t *log, pid_t pid);
void limd_log_set_str_level(limd_log_t *log, const char *level);
void limd_log_put(limd_log_t *log, enum limd_log_level_e level, const char *file, int line, const char *format, ...);
void limd_log_delete(limd_log_t *log);

#endif
