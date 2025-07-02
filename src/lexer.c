#include "lexer.h"
#include "imp_alloc.h"
#include "global.h"
#include <string.h>

Lexer*  lexer_init   (Options* options)
{
    Lexer* lexer   = imp_arena_alloc(arena, sizeof(Lexer));
    lexer->options = options;
    lexer->cursor  = 0;
    lexer->line    = 1;
    lexer->col     = 1;
    lexer->tokens  = make_tokenlist();

    
    
    return lexer;
}


char    lexer_peek   (Lexer* lexer)
{
    if (lexer->cursor >= strlen(lexer->source))
    {
	return EOF;
    }
    return lexer->source[lexer->cursor];
}

char    lexer_now    (Lexer* lexer)
{
}

void    lexer_advance(Lexer* lexer)
{
}
