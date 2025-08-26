#include "chunk.h"
#include "memory.h"
#include "value.h"
#include <stdint.h>

void Chunk_init(Chunk *chunk) {
    chunk->code = NULL;
    chunk->len = 0;
    chunk->cap = 0;
    chunk->lines = NULL;
    ValueArray_init(&chunk->constants);
}

void Chunk_free(Chunk *chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->cap);
    FREE_ARRAY(int, chunk->lines, chunk->cap);
    ValueArray_free(&chunk->constants);
    Chunk_init(chunk);
}

void Chunk_write(Chunk *chunk, uint8_t byte, int line) {
    if (chunk->len == chunk->cap) {
        int oldCapacity = chunk->cap;
        chunk->cap = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->cap);
        chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->cap);
    }

    chunk->code[chunk->len] = byte;
    chunk->lines[chunk->len] = line;
    chunk->len++;
}

size_t Chunk_addConstant(Chunk *chunk, Value value) {
    ValueArray_append(&chunk->constants, value);
    return chunk->constants.len - 1;
}
