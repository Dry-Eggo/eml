#include "parser.h"

#define pnowk(p) parser_now(p).kind
#define pnow(p)  parser_now(p)

Parser*  parser_init   (Options* options, const char* source, TokenList* list)
{
    Parser* parser      = imp_arena_alloc(arena, sizeof(Parser));
    parser->options     = options;
    parser->cursor      = 0;
    parser->program     = make_astlist();
    parser->source      = source;
    parser->tokens      = list;
    parser->token_count = tokenlist_size(parser->tokens);
    parser_parse(parser);
    return parser;
}

Token parser_peek(Parser* parser)
{
    if (parser->cursor + 1 < parser->token_count)
    {
	return tokenlist_get(parser->tokens, parser->cursor + 1);
    }
    // Last token is always set to TOKEN_EOF
    return tokenlist_get(parser->tokens, parser->token_count-1);
}

Token parser_now(Parser* parser)
{
    if (parser->cursor < parser->token_count)
    {
	return tokenlist_get(parser->tokens, parser->cursor);
    }
    return tokenlist_get(parser->tokens, parser->token_count-1);
}

void parser_advance(Parser* parser) {parser->cursor++;}

void parser_expect(Parser*  parser, TokenKind k)
{
    if (pnowk(parser) != k)
    {
	char buf[64];
	sprintf(buf, "%s:%d:%d error: Unexected token: '%s'", parser->options->infile, pnow(parser).span.line, pnow(parser).span.column, pnow(parser).lexme);
	early_fail(make_error(ERROR_FATAL, buf, 1));
    }
    parser_advance(parser);
}

void parser_parse(Parser* parser)
{
    while (pnowk(parser) != TOKEN_EOF)
    {
	// defaults to try and parse an expression
	// non-expression statments are
	//        ---------------------------------
	//     1 | import core;
	//     2 | module foo;
	//     3 |
	//     4 |
	switch(pnowk(parser))
	{
	    default:
		{
		    AstNode* expr = parse_expr(parser);
		    parser_expect(parser, TOKEN_SEMI);
		    astlist_add(parser->program, expr);
		} break;
	}
	
    }
}


AstNode* parse_expr(Parser* parser)
{
    // TODO: add parse chain
    return parse_atom(parser);
}

AstNode* parse_atom(Parser* parser)
{
    Token tok = pnow(parser);
    if (tok.kind == TOKEN_IDENTIFIER)
    {
	Span span_start = tok.span;
	const char* identifier = tok.lexme;
	parser_advance(parser);
	
	if (pnowk(parser) == TOKEN_OPEN_PAREN)
	{
	    parser_advance(parser);
	    AstList* args = make_astlist();
	    while (pnowk(parser) != TOKEN_CLOSE_PAREN)
	    {
		astlist_add(args, parse_expr(parser));
	    }
	    parser_expect(parser, TOKEN_CLOSE_PAREN);
	}
	
	return make_ident_node(identifier);
    }
    if (tok.kind == TOKEN_STRING)
    {
	Span span_start = tok.span;
	const char* string = tok.lexme;
	parser_advance(parser);
	return make_string_node(string);
    }
    char buf[64];
    sprintf(buf, "%s:%d:%d error: Unexected token: '%s'", parser->options->infile, pnow(parser).span.line, pnow(parser).span.column, pnow(parser).lexme);
    early_fail(make_error(ERROR_FATAL, buf, 1));
    /* UNREACHABLE */
    return NULL;
}
