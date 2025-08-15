#include "memory.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void *reallocate(void *p, size_t old_size, size_t new_size) {
    // printf("called reallocate with:\nold_size = %lu\nnew_size = %lu\n", old_size, new_size);
    if (new_size == 0) {
        free(p);
        return NULL;
    }

    void *result = realloc(p, new_size);
    assert(result != NULL && "Memory allocation failed");
    return result;
}
