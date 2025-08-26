#include <stdint.h>
#include <stdio.h>
#include "debug.h"
#include "chunk.h"


void Chunk_disassemble(Chunk *chunk, const char* name) {
    printf("== %s ==\n", name);

    int offset = 0;
    while (offset < chunk->len) offset = Instruction_disassemble(chunk, offset);
}

static int Instruction_simple(const char *name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int Instruction_constant(const char *name, Chunk *chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    Value_print(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

int Instruction_disassemble(Chunk *chunk, int offset) {
    printf("%04d ", offset);
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1])
        printf("   | ");
    else
        printf("%4d ", chunk->lines[offset]);

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_CONSTANT:
            return Instruction_constant("OP_CONSTANT", chunk, offset);
        case OP_ADD:
            return Instruction_simple("OP_ADD", offset);
        case OP_SUBTRACT:
            return Instruction_simple("OP_SUBTRACT", offset);
        case OP_MULTIPLY:
            return Instruction_simple("OP_MUTIPLY", offset);
        case OP_DIVIDE:
            return Instruction_simple("OP_DIVIDE", offset);
        case OP_NEGATE:
            return Instruction_simple("OP_NEGATE", offset);
        case OP_RETURN:
            return Instruction_simple("OP_RETURN", offset);
        default:
            printf("Unkown opcode\n");
            return offset + 1;
    }
}
