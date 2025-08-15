#include <stdbool.h>
#include "debug.h"

int main() {
    Chunk chunk;
    Chunk_init(&chunk);

    int constant = Chunk_addConstant(&chunk, 1.2);
    Chunk_write(&chunk, OP_CONSTANT, 123);
    Chunk_write(&chunk, constant, 123);
    Chunk_write(&chunk, OP_RETURN, 123);

    Chunk_disassemble(&chunk, "test");
    Chunk_free(&chunk);
    return 0;
}
