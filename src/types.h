#pragma once

#include "enums.h"

struct Value_;

typedef struct
{
    struct Value_** data;
    int             capacity;
    int             index;
} ValueList;

typedef struct Value_
{
    ValueKind   kind;
    union
    {
	const char*    string_value;
	int            int_value;
	struct Value_* ref_value;
    };
} Value;


Value* value_make_int(int v);
Value* value_make_string(const char* s);
Value* value_make_ref   (Value* v);
