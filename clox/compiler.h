#ifndef INCLUDE_CLOX_COMPILER_H_
#define INCLUDE_CLOX_COMPILER_H_

#include "chunk.h"

bool compile(const char *source, Chunk *chunk);

#endif  // INCLUDE_CLOX_COMPILER_H_
