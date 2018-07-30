#ifndef LIMD_CORE_H
#define LIMD_CORE_H

#if __GNUC__ >= 3
    #define likely(x) __builtin_expect(!!(x), 1)
    #define unlikely(x) __builtin_expect(!!(x), 0)
#else
    #define likely(x) (x)
    #define unlikely(x) (x)
#endif

#define limd_ok 0
#define limd_error -1


#endif
