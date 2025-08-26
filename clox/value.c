#include <stdio.h>
#include "value.h"
#include "memory.h"

void Value_print(Value value) {
    printf("%g", value);
}

void ValueArray_init(ValueArray *valueArray)
{
    valueArray->values = NULL;
    valueArray->len = 0;
    valueArray->cap = 0;
}

void ValueArray_free(ValueArray *valueArray) {
    FREE_ARRAY(Value, valueArray->values, valueArray->cap);
    ValueArray_init(valueArray);
}

void ValueArray_append(ValueArray *valueArray, Value value) {
    if (valueArray->len == valueArray->cap) {
        int oldCapacity = valueArray->cap;
        valueArray->cap = GROW_CAPACITY(oldCapacity);
        valueArray->values = GROW_ARRAY(Value, valueArray->values, oldCapacity, valueArray->cap);
    }

    valueArray->values[valueArray->len++] = value;
}
