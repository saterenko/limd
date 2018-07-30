#include "limd_log.h"
#include <sys/time.h>

limd_log_t *
limd_log_new(const char *file, int level) 
{
    limd_log_t *log = (limd_log_t *) malloc(sizeof(limd_log_t));
    if (!log) {
        fprintf(stderr, "can't malloc in %s:%d\n", __FILE__, __LINE__);
        return NULL;
    }
    log->fd = fopen(file, "a");
    if (!log->fd) {
        fprintf(stderr, "can't fopen %s in %s:%d\n", file, __FILE__, __LINE__);
        free(log);
        return NULL;
    }
    log->ts = 0;
    log->level = level;
    log->max_line_size = LIMD_LOG_STR_MAX_SIZE;
    pid_t pid = getpid();
    snprintf(log->pid_str, LIMD_LOG_PID_STR_SIZE, "(%lu)", (unsigned long) pid);

    return log;
}

void
limd_log_set_pid(limd_log_t *log, pid_t pid)
{
    snprintf(log->pid_str, LIMD_LOG_PID_STR_SIZE, "(%lu)", (unsigned long) pid);
}

void
limd_log_set_str_level(limd_log_t *log, const char *level)
{
    if (strcmp(level, "debug") == 0) {
        log->level = limd_log_level_debug;
    } else if (strcmp(level, "notice") == 0) {
        log->level = limd_log_level_info;
    } else if (strcmp(level, "warn") == 0) {
        log->level = limd_log_level_warn;
    } else {
        log->level = limd_log_level_error;
    }
}

void
limd_log_put(limd_log_t *log, enum limd_log_level_e level, const char *file, int line, const char *format, ...) 
{
    static const char *limd_log_levels[] = {"error", "warn", "info", "debug"};
    if (!log || log->level < level) {
        return;
    }
    /*  make time if need  */
    struct timeval tmv;
    gettimeofday(&tmv, NULL);
    if (tmv.tv_sec != log->ts) {
        struct tm tm;
        localtime_r(&tmv.tv_sec, &tm);
        snprintf(log->ts_str, LIMD_LOG_TIME_STR_SIZE, "[%04d-%02d-%02d %02d:%02d:%02d.", 
            (tm.tm_year + 1900), (tm.tm_mon + 1), tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        log->ts = tmv.tv_sec;
    }
    /*  write line begining  */
    char buf[LIMD_LOG_STR_SIZE];
    char *p = buf;
    char *begin = p;
    char *end = p + LIMD_LOG_STR_SIZE;
    if (file) {
        const char *k = strrchr(file, '/');
        if (!k) {
            k = file;
        } else {
            k++;
        }
        int rc = snprintf(p, LIMD_LOG_STR_SIZE, "%s%u] %s %s in %s:%d ", log->ts_str,
            (unsigned int) tmv.tv_usec, log->pid_str, limd_log_levels[level], k, line);
        p += rc;
    } else {
        int rc = snprintf(p, LIMD_LOG_STR_SIZE, "%s%u] %s %s ", log->ts_str,
            (unsigned int) tmv.tv_usec, log->pid_str, limd_log_levels[level]);
        p += rc;
    }
    /*  write message  */
    va_list args;
    while (1) {
        va_start(args, format);
        int rc = vsnprintf(p, end - p, format, args);
        va_end(args);
        if (rc < 0) {
            if (begin != buf) {
                free(begin);
            }
            return;
        }
        if (end - begin >= log->max_line_size) {
            p = begin + log->max_line_size - 1;
            break;
        }
        if (rc >= end - p) {
            size_t size = LIMD_LOG_STR_SIZE;
            while (size <= rc) {
                size = (size << 1) - (size >> 1);
            }
            char *nb = (char *) malloc(size);
            if (!nb) {
                if (begin != buf) {
                    free(begin);
                }
                return;
            }
            memcpy(nb, begin, p - begin);
            p = nb + (p - begin);
            end = nb + size;
            if (begin != buf) {
                free(begin);
            }
            begin = nb;
            continue;
        }
        p += rc;
        break;
    }
    *p++ = '\n';
    fwrite(begin, 1, p - begin, log->fd);
    if (begin != buf) {
        free(begin);
    }
    fflush(log->fd);
}

void
limd_log_delete(limd_log_t *log) 
{
    if (log) {
        if (log->fd) {
            fclose(log->fd);
        }
        free(log);
    }
}
