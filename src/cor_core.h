#ifndef COR_CORE_H
#define COR_CORE_H

#if __GNUC__ >= 3
    #define likely(x) __builtin_expect(!!(x), 1)
    #define unlikely(x) __builtin_expect(!!(x), 0)
#else
    #define likely(x) (x)
    #define unlikely(x) (x)
#endif

#define cor_ok 0
#define cor_error -1

#define cor_hash(key, c) ((unsigned int) key * 31 + c)

#endif
