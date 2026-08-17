#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "src/logging.h"
#include "./vec.h"



#define BASE_USE_AFTER_FREE_GUARD(this, return_op) \
    if (this->_is_disposed) { \
        char repr[VEC_REPR_LEN]; \
        Vec_repr(this, repr, sizeof(repr)); \
        log(L_ERROR, L_ARGS, "use %s after free", repr); \
        return_op; \
    }

#define USE_AFTER_FREE_GUARD_BY_RES_CODE(this, func_name) \
    BASE_USE_AFTER_FREE_GUARD(this, return func_name##_IsDisposed)

#define USE_AFTER_FREE_GUARD_BY_RESULT(this, func_name) \
    BASE_USE_AFTER_FREE_GUARD(this, return ERR(func_name, IsDisposed))

#define USE_AFTER_FREE_GUARD_FOR_PROC(this) \
    BASE_USE_AFTER_FREE_GUARD(this, return)

/* Special funcs */
void dispose_Vec(struct Vec *this) {
    USE_AFTER_FREE_GUARD_FOR_PROC(this)

    if (this->_buffer_need_free && this->_buffer != NULL) {
        free(this->_buffer);
    }

    this->_buffer = NULL;
    this->_capacity = 0;
    this->_is_disposed = true;
}

/* Properties */
static inline size_t _get_Vec_buffer_size(const struct Vec *this) {
    return this->_capacity * this->_item_size;
}


/* Read-only methods */
struct Vec_get_Result Vec_get(const struct Vec *this, size_t i) {
    USE_AFTER_FREE_GUARD_BY_RESULT(this, Vec_get)
    if (this->_count < i) {
        log(L_ERROR, L_ARGS, "i out of bounds, vec count is %d, i = %d", this->_count, i);
        return ERR(Vec_get, IndexOutOfBounds);
    }

    void *item_ptr = this->_buffer + (this->_item_size * i);
    return OK(Vec_get, item_ptr);
}

DEFINE_REPR_FOR(Vec, struct)


/* Mutate methods */
enum Vec_push_ResCode Vec_push(struct Vec *this, void* item) {
    USE_AFTER_FREE_GUARD_BY_RES_CODE(this, Vec_push)

    // Init if not initted
    if (this->_buffer == NULL) {
        void *buffer = malloc(this->_capacity * this->_item_size);
        if (buffer == NULL) {
            return Vec_push_MemoryOverflow;
        }
        this->_buffer = buffer;
        this->_buffer_need_free = true;
    }

    // Expand buffer if space in not enough
    if (this->_count + 1 >= this->_capacity) {
        size_t new_capacity = ROUND_UP_TO_8(this->_capacity + (this->_capacity / 2));
        size_t new_size = new_capacity * this->_item_size;

        void *new_buffer = malloc(new_size);
        if (new_buffer == NULL) {
            return Vec_push_MemoryOverflow;
        }
        memcpy(new_buffer, this->_buffer, _get_Vec_buffer_size(this));

        this->_buffer = new_buffer;
        this->_capacity = new_capacity;
    }

    size_t used_space = this->_item_size * this->_count;
    void *new_item_addr = this->_buffer + used_space;
    memcpy(new_item_addr, item, this->_item_size);
    this->_count++;

    return Vec_push_Success;
}

enum Vec_push_ResCode Vec_push_several(struct Vec *this, void *items, size_t items_count) {
    USE_AFTER_FREE_GUARD_BY_RES_CODE(this, Vec_push)

    // Plug
    for (size_t i = 0; i < items_count; i++) {
        void *item = items + (i * this->_item_size);
        enum Vec_push_ResCode res = Vec_push(this, item);
        if (res == Vec_push_MemoryOverflow) return res;
        assert(res == Vec_push_Success);
    }

    return Vec_push_Success;
}

void Vec_clear(struct Vec *this) {
    USE_AFTER_FREE_GUARD_FOR_PROC(this)

    this->_count = 0;
}
