#ifndef INCLUDE_CLOX_DEBUG_H_
#define INCLUDE_CLOX_DEBUG_H_

#include "chunk.h"

void Chunk_disassemble(Chunk *chunk, const char* name);
int Instruction_disassemble(Chunk *chunk, int offset);

#endif  // INCLUDE_CLOX_DEBUG_H_
