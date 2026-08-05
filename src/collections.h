#pragma once
#include <stdlib.h>
#include "src/common.h"


struct Iterator {
    size_t item_size;
};
struct IterationStep {
    bool is_exhausted;
};
struct IterationStep *next(struct Iterator *self);



struct Vec {
    size_t _item_size;
    size_t _items_count;
    void *_buffer;
    // Current capacity in items
    size_t _capacity;
    // Prevent use-after-free;
    bool _is_destructed;
};

enum VecResult {
    VecResult_Success,
    VecResult_Destructed,
    VecResult_MemoryOverflow,
};


struct Vec new_Vec(size_t item_size);
struct Vec new_Vec_with_start_capacity(size_t item_size, size_t capacity);
// Free heap mem, mark Vec as destructed
void del_Vec(struct Vec *self);

void Vec_append(struct Vec *self, void* item);
struct Iterator iter_Vec(const struct Vec *self);
// Return ref to item in Vec
const void *Vec_get(const struct Vec *self, size_t i);
// Return ref to item in Vec.
// Special variant for explicit items mutation
void *Vec_get_for_change(const struct Vec *self, size_t i);
size_t Vec_count(const struct Vec *self);
