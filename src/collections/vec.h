#pragma once
#include <stdlib.h>
#include <assert.h>

#include "src/generic/repr.h"
#include "src/common.h"

#if FALSE
/* Iterator def */
struct Iterator {
    size_t item_size;
};

struct IterationStep {
    // NULL if exhausted
    void *item;
    bool is_exhausted;
};

struct IterationStep *next(struct Iterator *this);

struct Iterator Vec_iter(const struct Vec *this);
#endif


/* Vec def */
struct Vec {
    size_t _item_size;
    size_t _count;
    void *_buffer;
    // Current capacity in items
    size_t _capacity;
    // Prevent use-after-free;
    bool _is_disposed;
    bool _buffer_need_free;
};


/* Special funcs */
// Given buffer can be NULL
static inline struct Vec new_Vec_with_buffer(size_t item_size, void *buffer, size_t capacity, bool need_free) {
    assert(ROUND_UP_TO_8(capacity) == capacity);
    return (struct Vec) {
        ._item_size = item_size,
        ._count = 0,
        ._buffer = buffer,
        ._capacity = capacity,
        ._buffer_need_free = need_free,
        ._is_disposed = false,
    };
}

static inline struct Vec new_Vec(size_t item_size) {
    return new_Vec_with_buffer(item_size, NULL, 0, false);
}

// Free allocated mem, mark Vec as disposed
void dispose_Vec(struct Vec *this);

/* Properties */
static inline size_t get_Vec_count(const struct Vec *this) {
    return this->_count;
}

/* Read-only Methods */
enum Vec_get_ResCode {
    Vec_get_Success,
    Vec_get_IsDisposed,
    // Also returned if not initted, because it's equivalent zero size
    Vec_get_IndexOutOfBounds,
};
DEF_RESULT(Vec_get, void*);

struct Vec_get_Result Vec_get(const struct Vec *this, size_t i);

// You can known need buffer length at comp-time by `REPR_LEN`
// macro (`REPR_LEN` + 1 for null-terminator)
DECLARE_REPR_FOR(Vec, struct);
#define VEC_REPR_LEN REPR_LEN(struct Vec)

/* Mutate methods */
enum Vec_push_ResCode {
    Vec_push_Success,
    Vec_push_IsDisposed,
    Vec_push_MemoryOverflow,
};
enum Vec_push_ResCode Vec_push(struct Vec *this, void* item);
enum Vec_push_ResCode Vec_push_several(struct Vec *this, void *items, size_t items_count);

// Remove all items without deallocating
void Vec_clear(struct Vec *this);
