#pragma once
#include "src/collections/vec.h"

struct Freeable {
    void *ptr;
    void (*free)(void*);
};

static inline void Freeable_free(const struct Freeable *this) {
    this->free(this->ptr);
}

// `<Item = struct Freeable>`
typedef struct Vec FreeablesVec;
