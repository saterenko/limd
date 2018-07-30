#pragma once

#include <cstdint>
#include <cstring>
#include <sys/types.h>

#include <stdexcept>

namespace limd
{

class PoolException: public std::runtime_error
{
public:
    explicit PoolException(const std::string& what) : std::runtime_error(what) {};
    explicit PoolException(const char *what) : std::runtime_error(what) {};
};


class Pool
{
public:
    struct Slab
    {
        uint8_t *last;
        uint8_t *end;
        int failed;
        Slab *next;
    };

    struct Large
    {
        Large *next;
        void *data;
        size_t size;
    };

    Pool(size_t size);
    ~Pool();
    void *alloc(size_t size, bool allign = true);
    void *calloc(size_t size, bool allign = true);
    void free(void *p);
    void reset();
    size_t allocated() const;
    size_t used() const;

protected:
    Slab *newSlab();
    void *allignPtr(void *p)
    {
        return (void *)(((uintptr_t) p + ((uintptr_t) sizeof(unsigned long) - 1)) & ~((uintptr_t) sizeof(unsigned long) - 1));
    }

    size_t size_;
    size_t maxAlloc_;
    Slab *root_;
    Slab *current_;
    Large *large_;
};

} // end of limd namespace
