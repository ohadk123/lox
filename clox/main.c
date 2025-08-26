#include "common.h"
#include "chunk.h"
#include "vm.h"

int main() {
    VM_init();

    Chunk chunk;
    Chunk_init(&chunk);

    int constant = Chunk_addConstant(&chunk, 1.2);
    Chunk_write(&chunk, OP_CONSTANT, 123);
    Chunk_write(&chunk, constant, 123);

    constant = Chunk_addConstant(&chunk, 3.4);
    Chunk_write(&chunk, OP_CONSTANT, 123);
    Chunk_write(&chunk, constant, 123);

    Chunk_write(&chunk, OP_ADD, 123);

    constant = Chunk_addConstant(&chunk, 5.6);
    Chunk_write(&chunk, OP_CONSTANT, 123);
    Chunk_write(&chunk, constant, 123);

    Chunk_write(&chunk, OP_DIVIDE, 123);
    Chunk_write(&chunk, OP_NEGATE, 123);
    Chunk_write(&chunk, OP_RETURN, 123);

    // Chunk_disassemble(&chunk, "test");
    interpret(&chunk);
    VM_free();
    Chunk_free(&chunk);
    return 0;
}
