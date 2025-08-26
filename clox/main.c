#include "common.h"
#include "vm.h"
#include <stdarg.h>
#include <stdlib.h>

void lox_abort(int code, const char *msg, ...) {
    va_list args;
    fprintf(stderr, "Error: ");
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(code);
}

static void repl() {
    char line[1024];
    for (;;) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        interpret(line);
    }
}

static char *readFile(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) lox_abort(74, "Could not open file \"%s\".", path);

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(fileSize + 1);
    if (buffer == NULL) lox_abort(74, "Not enough memory to read \"%s\".", path);
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) lox_abort(74, "Count not read file \"%s\".", path);
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void runFile(const char *path) {
    char *source = readFile(path);
    InterpretResult result = interpret(source);
    free(source);

    switch (result) {
        case INTERPRET_OK:              return;
        case INTERPRET_COMPILE_ERROR:   exit(65);
        case INTERPRET_RUNTIME_ERROR:   exit(70);
    }
}

int main(int argc, char *argv[]) {
    VM_init();

    switch (argc) {
        case 1:     repl(); break;
        case 2:     runFile(argv[1]); break;
        default:    printf("Usage: clox [path]\n"); return 69;
    }

    VM_free();
    return 0;
}
