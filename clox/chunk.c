#include "chunk.h"
#include "memory.h"
#include "value.h"

void Chunk_init(Chunk *chunk) {
    chunk->code = NULL;
    chunk->len = 0;
    chunk->cap = 0;
    chunk->lines = NULL;
    ValueArray_init(&chunk->constants);
}

void Chunk_free(Chunk *chunk) {
    reallocate(chunk->code, chunk->cap, 0);
    reallocate(chunk->lines, chunk->cap, 0);
    ValueArray_free(&chunk->constants);
    Chunk_init(chunk);
}

void Chunk_write(Chunk *chunk, uint8_t byte, int line) {
    if (chunk->len == chunk->cap) {
        chunk->cap = chunk->cap == 0 ? MIN_CHUNK_CAP : chunk->cap * 2;
        
        chunk->code = reallocate(chunk->code, chunk->len, chunk->cap);
        chunk->lines = reallocate(chunk->lines, chunk->len, chunk->cap);
    }

    chunk->code[chunk->len] = byte;
    chunk->lines[chunk->len] = line;
    chunk->len++;
}

size_t Chunk_addConstant(Chunk *chunk, Value value) {
    ValueArray_append(&chunk->constants, value);
    return chunk->constants.len - 1;
}
