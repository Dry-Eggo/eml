#pragma once

#include "options.h"
#include "misc/containers.h"
#include "node.h"
#include "imp_alloc.h"
#include "global.h"

typedef struct
{
    int         cursor;
    int         token_count;
    Options*    options;
    AstList*    program;
    TokenList*  tokens;
    const char* source;
} Parser;

Parser*    parser_init        (Options* options, const char* source, TokenList* list);
Token      parser_peek        (Parser*  parser);
Token      parser_now         (Parser*  parser);
void       parser_advance     (Parser*  parser);
void       parser_expect      (Parser*  parser, TokenKind k);

void       parser_parse       (Parser*  parser);
AstNode*   parse_expr         (Parser*  parser);
AstNode*   parse_atom         (Parser*  parser);
AstNode*   parse_additive     (Parser*  parser);
AstNode*   parse_term         (Parser*  parser);
AstNode*   parse_logical_or   (Parser*  parser);
AstNode*   parse_logical_and  (Parser*  parser);
