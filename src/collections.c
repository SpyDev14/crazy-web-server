#include <stdlib.h>
#include <stdio.h>

#include "src/common.h"
#include "./collections.h"


// Special funcs
inline struct Vec new_Vec_with_start_capacity(size_t item_size, size_t start_capacity) {
    return (struct Vec){
        ._item_size = item_size,
        ._items_count = 0,
        // NULL mean is not initted
        ._buffer = NULL,
        ._capacity = start_capacity,
        // Prevent use-after-free
        ._is_destructed = false,
    };
}

inline struct Vec new_Vec(size_t item_size) {
    return new_Vec_with_start_capacity(item_size, 0);
}

struct Iterator iter_Vec(const struct Vec *self) {

}

// Getters
inline size_t Vec_count(const struct Vec *self) {
    return self->_items_count;
}

// Read-only methods
void *get(const struct Vec *self, size_t i) {

}

// Mutate methods
void Vec_append(struct Vec *self, void* item) {

}
