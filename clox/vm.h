#ifndef INCLUDE_CLOX_VM_H_
#define INCLUDE_CLOX_VM_H_

#include "chunk.h"

#define STACK_MAX 256

typedef struct {
    Chunk *chunk;
    uint8_t *ip;
    Value stack[STACK_MAX];
    Value *stackTop;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void VM_init();
void VM_free();

InterpretResult interpret(const char *source);
void push(Value value);
Value pop();

#endif  // INCLUDE_CLOX_VM_H_
