#ifndef INCLUDE_CLOX_VALUE_H_
#define INCLUDE_CLOX_VALUE_H_

#include <stdio.h>
#include "common.h"

typedef double Value;

void Value_print(Value value);

typedef struct {
    Value *values;
    size_t len;
    size_t cap;
} ValueArray;

void ValueArray_init(ValueArray *valueArray);
void ValueArray_free(ValueArray *valueArray);
void ValueArray_append(ValueArray *valueArray, Value value);

#endif  // INCLUDE_CLOX_VALUE_H_
