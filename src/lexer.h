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

Lexer*  lexer_init         (Options* options);
char    lexer_peek         (Lexer* lexer);
char    lexer_now          (Lexer* lexer);
char    lexer_advance      (Lexer* lexer);
void    lexer_lex          (Lexer* lexer);
void    lexer_skip_ws      (Lexer* lexer);
void    lexer_parse_word   (Lexer* lexer);
void    lexer_parse_number (Lexer* lexer);
void    lexer_parse_string (Lexer* lexer);
