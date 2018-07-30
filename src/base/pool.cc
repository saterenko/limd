#include "pool.h"

using namespace limd;

Pool::Pool(size_t size) : root_(nullptr), current_(nullptr), large_(nullptr)
{
    size_ = size < 256 ? 256 : size;
    maxAlloc_ = (size_ < 4095) ? size_ : 4095;
    if (maxAlloc_ + sizeof(Slab) > size_) {
        maxAlloc_ = size_ - sizeof(Slab);
    }
    root_ = current_ = newSlab();
}

Pool::~Pool()
{
    Slab *p, *n;
    for (p = root_, n = root_->next; ; p = n, n = n->next) {
        ::free(p);
        if (!n) {
            break;
        }
    }
    for (Large *l = large_; l; l = l->next) {
        if (l->data) {
            ::free(l->data);
        }
    }
}

void *
Pool::alloc(size_t size, bool allign)
{
    if (size <= maxAlloc_) {
        /*  alloc small  */
        Slab *p = current_;
        do {
            uint8_t *m = allign ? (uint8_t *) allignPtr(p->last) : p->last;
            if ((size_t ) (p->end - m) >= size) {
                p->last = m + size;
                return m;
            }
            p = p->next;
        } while (p);
        /*  alloc new slab  */
        Slab *s = newSlab();
        for (p = current_; p->next; p = p->next) {
            if (p->failed++ > 4) {
                current_ = p->next;
            }
        }
        p->next = s;
        /**/
        uint8_t *m = allign ? (uint8_t *) allignPtr(s->last) : s->last;
        s->last = m + size;
        return m;
    }
    /*  alloc large  */
    uint8_t *m = (uint8_t *) malloc(size);
    if (!m) {
        throw PoolException("can't malloc");
    }
    int n = 0;
    for (Large *l = large_; l; l = l->next) {
        if (!l->data) {
            l->data = m;
            return m;
        }
        if (n++ > 3) {
            break;
        }
    }
    Large *l = (Large *) alloc(sizeof(Large));
    if (!l) {
        ::free(m);
        throw PoolException("can't malloc");
    }
    l->data = m;
    l->next = large_;
    l->size = size;
    large_ = l;
    return m;
}

void *
Pool::calloc(size_t size, bool allign)
{
    void *m = alloc(size, allign);
    memset(m, 0, size);
    return m;
}

void
Pool::free(void *p)
{
    for (Large *l = large_; l; l = l->next) {
        if (p == l->data) {
            ::free(l->data);
            l->data = nullptr;
            l->size = 0;
            return;
        }
    }
}

void
Pool::reset()
{
    for (Slab *p = root_; p; p = p->next) {
        p->last = (uint8_t *) (p + 1);
        p->failed = 0;
    }
    current_ = root_;
    for (Large *l = large_; l; l = l->next) {
        if (l->data) {
            ::free(l->data);
            l->size = 0;
        }
    }
    large_ = nullptr;
}

size_t
Pool::allocated() const
{
    size_t size = 0;
    for (Slab *p = root_; p; p = p->next) {
        size += size_;
    }
    for (Large *p = large_; p; p = p->next) {
        size += p->size;
    }
    return size;
}

size_t
Pool::used() const
{
    size_t size = 0;
    for (Slab *p = root_; p; p = p->next) {
        size += p->last - (uint8_t *) p;
    }
    for (Large *p = large_; p; p = p->next) {
        size += p->size;
    }
    return size;
}

Pool::Slab *
Pool::newSlab()
{
    Slab *s = (Slab *) malloc(size_);
    if (!s) {
        throw PoolException("can't malloc");
    }
    s->last = (uint8_t *) (s + 1);
    s->end = (uint8_t *) s + size_;
    s->failed = 0;
    s->next = nullptr;
    return s;
}

