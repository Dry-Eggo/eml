#pragma once

#include "options.h"
#include "misc/containers.h"
#include "node.h"
#include "imp_alloc.h"
#include "global.h"

typedef struct {
    int         cursor;
    int         token_count;
    Options*    options;
    AstList*    program;
    TokenList*  tokens;
    std::string source;
} Parser;

Parser*    parser_init        (Options* options, std::string source, TokenList* list);
Token      parser_peek        (Parser*  parser);
Token      parser_now         (Parser*  parser);
void       parser_advance     (Parser*  parser);
void       parser_expect      (Parser*  parser, TokenKind k);
bool       parser_match       (Parser*  parser, TokenKind k);
void       parser_parse       (Parser*  parser);

AstPtr   parse_expr         (Parser*  parser);
AstPtr   parse_atom         (Parser*  parser);
AstPtr   parse_additive     (Parser*  parser);
AstPtr   parse_term         (Parser*  parser);
AstPtr   parse_postfix      (Parser*  parser);
AstPtr   parse_logical_or   (Parser*  parser);
AstPtr   parse_logical_and  (Parser*  parser);
AstPtr   parse_body         (Parser*  parser);
