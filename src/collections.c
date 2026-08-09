#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "src/common.h"
#include "src/generic.h"
#include "src/logging.h"
#include "./collections.h"
#include "collections.h"


/* Special funcs */
void destruct_Vec(struct Vec *self) {
    if (self->_is_destructed) {
        char *repr = Vec_repr(self);
        log(L_ERROR, "Vec %p is destructed already\n", (void *)self);
        free(repr);
        return;
    }

    if (self->_buffer != NULL && self->_buffer_need_free) {
        free(self->_buffer);
    }

    self->_buffer = NULL;
    self->_capacity = 0;
    self->_is_destructed = true;
}

/* Properties */
static inline size_t _get_Vec_buffer_size(const struct Vec *self) {
    return self->_capacity * self->_item_size;
}

void *get_Vec_buffer_copy(const struct Vec *self) {
    size_t buff_sz = _get_Vec_buffer_size(self);
    void *new_buff = malloc(buff_sz);
    if (new_buff == NULL)
    return NULL;

    memcpy(new_buff, self->_buffer, buff_sz);
    return new_buff;
}


/* Read-only methods */
struct Vec_get_Result Vec_get(const struct Vec *self, size_t i) {
    if (self->_is_destructed) {
        return ERR(Vec_get, Vec_get_IsDestructed);
    }
    if (self->_count < i) {
        return ERR(Vec_get, Vec_get_IndexOutOfBounds);
    }

    void *item_ptr = self->_buffer + (self->_item_size * i);
    return OK(Vec_get, item_ptr);
}

DEF_REPR_IMPL(Vec, struct)


/* Mutate methods */
enum Vec_push_ResCode Vec_push(struct Vec *self, void* item) {
    if (self->_is_destructed) {
        return Vec_push_IsDestructed;
    }

    // Init if not initted
    if (self->_buffer == NULL) {
        void *buffer = malloc(self->_capacity * self->_item_size);
        if (buffer == NULL) {
            return Vec_push_MemoryOverflow;
        }
        self->_buffer = buffer;
        self->_buffer_need_free = true;
    }

    // Expand buffer if space in not enough
    if (self->_count + 1 >= self->_capacity) {
        size_t new_capacity = ROUND_UP_TO_8(self->_capacity + (self->_capacity / 2));
        size_t new_size = new_capacity * self->_item_size;

        void *new_buffer = malloc(new_size);
        if (new_buffer == NULL) {
            return Vec_push_MemoryOverflow;
        }
        memcpy(new_buffer, self->_buffer, _get_Vec_buffer_size(self));

        self->_buffer = new_buffer;
        self->_capacity = new_capacity;
    }

    size_t used_space = self->_item_size * self->_count;
    void *new_item_addr = self->_buffer + used_space;
    memcpy(new_item_addr, item, self->_item_size);
    self->_count++;

    return Vec_push_Success;
}

enum Vec_push_ResCode Vec_push_several(struct Vec *self, void **items, size_t items_count) {
    // Will be reworked
    for (size_t i = 0; i < items_count; i++) {
        int res = Vec_push(self, items[i * self->_item_size]);
        if (res != Vec_push_Success) return res;
    }

    return Vec_push_Success;
}
