#include "chunk.h"
#include "debug.h"
#include "value.h"
#include <assert.h>
#include <stdio.h>
#include "vm.h"
VM vm;

static void Stack_reset() {
    vm.stackTop = vm.stack;
}

void VM_init() {
    Stack_reset();
}

void VM_free() {
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    do { \
        double b = pop(); \
        double a = pop(); \
        push(a op b); \
    } while (0)

    for(;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("        ");
        for (Value *slot = vm.stack; slot < vm.stackTop; slot++) {
            printf("[ ");
            Value_print(*slot);
            printf(" ]");
        }
        printf("\n");
        Instruction_disassemble(vm.chunk, (int) (vm.ip - vm.chunk->code));
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT:
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            case OP_ADD:        BINARY_OP(+); break;
            case OP_SUBTRACT:   BINARY_OP(-); break;
            case OP_MULTIPLY:   BINARY_OP(*); break;
            case OP_DIVIDE:     BINARY_OP(/); break;
            case OP_NEGATE:     push(-pop()); break;
            case OP_RETURN:
                Value_print(pop());
                printf("\n");
                return INTERPRET_OK;
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(Chunk *chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}

void push(Value value) {
    assert(vm.stackTop <= vm.stack + STACK_MAX && "Stack Overflow");
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    assert(vm.stackTop > vm.stack && "Stack Underflow");
    vm.stackTop--;
    return *vm.stackTop;
}
