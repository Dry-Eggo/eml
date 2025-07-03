#pragma once

#include "enums.h"

typedef struct
{
    int          line;
    int          column;
    int          colend;
} Span;

typedef struct
{
    TokenKind    kind;
    const char*  lexme;
    Span         span;
} Token;

Token make_token   (TokenKind kind, const char* lexme, Span span);
Span  make_span    (int line, int column, int colend);
Span  merge_span   (Span s1, Span s2);
