#include "parser.h"
#include "enums.h"
#include "node.h"

#define pnowk(p) parser_now(p).kind
#define pnow(p) parser_now(p)

Parser *parser_init(Options *options, std::string source, TokenList *list) {
    Parser *parser = (Parser*)imp_arena_alloc(arena, sizeof(Parser));
    parser->options = options;
    parser->cursor = 0;
    parser->source = source;
    parser->tokens = list;
    parser->token_count = tokenlist_size(parser->tokens);
    parser_parse(parser);
    return parser;
}

Token parser_peek(Parser *parser) {
    if (parser->cursor + 1 < parser->token_count) {
	return tokenlist_get(parser->tokens, parser->cursor + 1);
    }
    // Last token is always set to TOKEN_EOF
    return tokenlist_get(parser->tokens, parser->token_count - 1);
}

Token parser_now(Parser *parser) {
    if (parser->cursor < parser->token_count) {
	return tokenlist_get(parser->tokens, parser->cursor);
    }
    return tokenlist_get(parser->tokens, parser->token_count - 1);
}

void parser_advance(Parser *parser) { parser->cursor++; }

void parser_expect(Parser *parser, TokenKind k) {
    if (pnowk(parser) != k) {
	char buf[64];
	sprintf(buf, "%s:%d:%d error: Unexpected token: '%s'",
        parser->options->infile, pnow(parser).span.line,
        pnow(parser).span.column, pnow(parser).lexme.c_str());
	early_fail(make_error(ERROR_FATAL, buf, 1));
    }
    parser_advance(parser);
}

bool parser_match(Parser *parser, TokenKind k) {
    if (pnowk(parser) != k) {
	return false;
    }
    return true;
}

void parser_parse(Parser *parser) {
    int program_size = 0;
    while (pnowk(parser) != TOKEN_EOF) {
	// defaults to try and parse an expression
	// non-expression statments are
	//        ---------------------------------
	//     1 | import core;
	//     2 | module foo;
	//     3 |
	//     4 |
	switch (pnowk(parser)) {
	default: {
	    auto expr = parse_expr(parser);
	    if (expr->kind != EXPR_IF) parser_expect(parser, TOKEN_SEMI);
	    parser->program.push_back(std::move(expr));
	    program_size++;
	} break;
    }
}
}

AstPtr parse_body(Parser*  parser) {
    Span start = pnow(parser).span;
    AstList body = {};
    while (!parser_match(parser, TOKEN_EOF) && !parser_match(parser, TOKEN_END) && !parser_match(parser, TOKEN_ELSE)) {
	auto expr = parse_expr(parser);
	if (expr->kind != EXPR_IF) parser_expect(parser, TOKEN_SEMI);
	body.push_back(std::move(expr));
    }
    Span end = pnow(parser).span;
    return make_body_node(std::move(body), merge_span(start, end));
}

AstPtr parse_expr(Parser *parser) {
    // TODO: add parse chain
    return parse_logical_or(parser);
}

AstPtr parse_logical_or(Parser *parser) {
    auto lhs = parse_logical_and(parser);
    return lhs;
}
AstPtr parse_logical_and(Parser *parser) {
    auto lhs = parse_equality(parser);
    return lhs;
}

AstPtr parse_equality (Parser* parser) {
    Span start = pnow(parser).span;
    auto lhs = parse_additive(parser);
    while (parser_match(parser, TOKEN_EQEQ)) {
	auto op = BINOP_EQ;
	parser_advance(parser);
	Span end = pnow(parser).span;
	auto rhs = parse_expr(parser);
	lhs = make_binary_op(std::move(lhs), std::move(rhs), op, merge_span(start, end));
    }
    return lhs;
}

AstPtr parse_additive(Parser *parser) {
    Span start = pnow(parser).span;
    auto lhs = parse_term(parser);
    while (parser_match(parser, TOKEN_ADD) || parser_match(parser, TOKEN_SUB)) {
	BinaryOp op = (pnowk(parser) == TOKEN_SUB) ? BINOP_SUB : BINOP_ADD;
	parser_advance(parser);
	auto rhs = parse_expr(parser);
	Span end = pnow(parser).span;
	lhs = make_binary_op(std::move(lhs), std::move(rhs), op, merge_span(start, end));
    }
    return lhs;
}

AstPtr parse_term(Parser *parser) {
    auto lhs = parse_postfix(parser);
    return lhs;
}

AstPtr parse_postfix(Parser *parser) {
    Span start = pnow(parser).span;
    auto lhs = parse_atom(parser);
    if (parser_match(parser, TOKEN_EQ) || parser_match(parser, TOKEN_COLEQ))
    {
	bool mut = (pnowk(parser) == TOKEN_COLEQ)? true: false;
	parser_advance(parser);
	auto value = parse_expr(parser);
	Span end = pnow(parser).span;
	return make_binding_node(std::move(lhs), std::move(value), mut, merge_span(start, end));
    }
    while (parser_match(parser, TOKEN_DOT)) {
	if (pnowk(parser) == TOKEN_DOT) {
	    parser_advance(parser);
	    auto field = parse_expr(parser);
	    Span end = pnow(parser).span;
	    lhs = make_field_access(std::move(lhs), std::move(field), merge_span(start, end));
	}
    }
    return lhs;
}

AstPtr parse_atom(Parser *parser) {
    Token tok = pnow(parser);
    if (tok.kind == TOKEN_IDENTIFIER) {
	Span start = tok.span;
	std::string identifier = tok.lexme;
	parser_advance(parser);
	AstPtr ident_node = make_ident_node(identifier, start);
	if (pnowk(parser) == TOKEN_OPEN_PAREN) {
	    parser_advance(parser);
	    AstList args = {};
	    while (pnowk(parser) != TOKEN_CLOSE_PAREN) {
		args.push_back(parse_expr(parser));
		if (parser_match(parser, TOKEN_COMMA))
		{
		    parser_advance(parser);
		}
	    }
	    Span end = pnow(parser).span;
	    parser_expect(parser, TOKEN_CLOSE_PAREN);
	    return make_call_node(std::move(ident_node), std::move(args), merge_span(start, end));
	}

	return ident_node;
    }
    
    if (tok.kind == TOKEN_TRUE || tok.kind == TOKEN_FALSE) {
	bool value = tok.kind == TOKEN_TRUE;
	auto span  = tok.span;
	parser_advance(parser);
	return make_bool_node(value, span);
    }
    
    if (tok.kind == TOKEN_IF) {
	Span start = tok.span;
	parser_advance(parser);
	AstPtr cond = parse_expr(parser);

	parser_expect(parser, TOKEN_THEN);
	
	AstPtr body = NULL;
	AstPtr else_ = NULL;

	parser_expect(parser, TOKEN_COLON);
	
	body = parse_body(parser);

	if (parser_match(parser, TOKEN_ELSE)) {
	    parser_advance(parser);
	    if (parser_match(parser, TOKEN_IF)) {
		AstPtr nested_if = parse_expr(parser);
		AstList b_ = {};
		b_.push_back(std::move(nested_if));
		Span end = pnow(parser).span;
		else_ = make_body_node(std::move(b_), merge_span(start, end));
	    } else {
		parser_expect(parser, TOKEN_COLON);
		else_ = parse_body(parser);
	    }
	}
		
	Span end = pnow(parser).span;
	parser_expect(parser, TOKEN_END);
	return make_if_node(std::move(cond), std::move(body), std::move(else_), merge_span(start, end));
    }
    if (tok.kind == TOKEN_STRING) {
	Span start = tok.span;
	auto string = tok.lexme;
	parser_advance(parser);
	return make_string_node(imp_arena_strdup(arena, string.data()), start);
    }
    if (tok.kind == TOKEN_INT) {
	Span span_start = tok.span;
	int value = strtol(tok.lexme.c_str(), NULL, 0);
	parser_advance(parser);
	return make_int_node(value, span_start);
    }

    if (tok.kind == TOKEN_KNIL) {
	Span span_start = tok.span;
	parser_advance(parser);
	return make_nil_node(span_start);
    }    
    
    char buf[64];
    sprintf(buf, "%s:%d:%d error: Unexpected token: '%s'", parser->options->infile,
    pnow(parser).span.line, pnow(parser).span.column, pnow(parser).lexme.c_str());
    early_fail(make_error(ERROR_FATAL, buf, 1));
    /* UNREACHABLE */
    return NULL;
}
