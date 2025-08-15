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
    reallocate(valueArray->values, valueArray->cap, 0);
    ValueArray_init(valueArray);
}

void ValueArray_append(ValueArray *valueArray, Value value) {
    if (valueArray->len == valueArray->cap) {
        valueArray->cap = valueArray->cap == 0 ? 8 : valueArray->cap * 2;
        valueArray->values = reallocate(valueArray->values, valueArray->len, valueArray->cap);
    }

    valueArray->values[valueArray->len++] = value;
}
