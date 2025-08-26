#ifndef INCLUDE_CLOX_CHUNK_H_
#define INCLUDE_CLOX_CHUNK_H_

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_RETURN,

    OP_NUM
} OpCode;

typedef struct {
    uint8_t *code;
    size_t len;
    size_t cap;
    int *lines;
    ValueArray constants;
} Chunk;

#define MIN_CHUNK_CAP 8

void Chunk_init(Chunk *chunk);
void Chunk_free(Chunk *chunk);
void Chunk_write(Chunk *chunk, uint8_t byte, int line);
size_t Chunk_addConstant(Chunk *chunk, Value value);

#endif  // INCLUDE_CLOX_CHUNK_H_
