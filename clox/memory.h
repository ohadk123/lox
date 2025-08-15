#ifndef INCLUDE_CLOX_MEMORY_H
#define INCLUDE_CLOX_MEMORY_H

#include "common.h"

void *reallocate(void *p, size_t old_size, size_t new_size);

#define lox_free(p) reallocate(p, 0, 0)

#endif // INCLUDE_CLOX_MEMORY_H