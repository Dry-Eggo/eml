#pragma once

#include "enums.h"

typedef struct
{
    int          code;
    const char*  message;
    ErrorKind    kind;
} Error;


Error* make_error     (ErrorKind kind, const char* message, int code);
void   early_fail     (Error* e);
void   maybe_fail    (Error* e);

