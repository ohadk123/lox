#ifndef INCLUDE_CLOX_MEMORY_H
#define INCLUDE_CLOX_MEMORY_H

#include "common.h"

void *reallocate(void *p, size_t old_size, size_t new_size);

#define lox_free(p) reallocate(p, 0, 0)

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : capacity * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

#endif // INCLUDE_CLOX_MEMORY_H
