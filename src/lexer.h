#pragma once

#include "enums.h"
#include "options.h"
#include "token.h"
#include "./misc/containers.h"

typedef struct
{
    int          cursor;
    int          line;
    int          col;
    const char*  source;
    Options*     options;
    TokenList*   tokens;
} Lexer;

Token make_token   (TokenKind kind, const char* lexme, Span span);
Span  make_span    (int line, int column, int offset);
Span  merge_span   (Span s1, Span s2);

Lexer*  lexer_init   (Options* options);
char    lexer_peek   (Lexer* lexer);
char    lexer_now    (Lexer* lexer);
void    lexer_advance(Lexer* lexer);
