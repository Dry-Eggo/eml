#pragma once

typedef struct
{
    int          line;
    int          column;
    int          offset;
} Span;

typedef struct
{
    const char*  lexme;
    Span         span;
} Token;
