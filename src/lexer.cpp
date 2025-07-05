#include "lexer.h"
#include "enums.h"
#include "global.h"
#include "imp_alloc.h"
#include "token.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define TMP_BUFFER_SIZE 256

Lexer *lexer_init(Options *options) {
    Lexer *lexer = (Lexer*)imp_arena_alloc(arena, sizeof(Lexer));
    lexer->options = options;
    lexer->cursor = 0;
    lexer->line = 1;
    lexer->col = 1;
    lexer->tokens = make_tokenlist();

    lexer->source = readAllFile(options->infile);
    lexer_lex(lexer);
    return lexer;
}

char lexer_peek(Lexer *lexer) {
    if (lexer->cursor + 1 < lexer->source.size()) {
	return lexer->source[lexer->cursor + 1];
    }
    return EOF;
}

char lexer_now(Lexer *lexer) {
    if (lexer->cursor >= lexer->source.size()) {
	return EOF;
    }
    return lexer->source[lexer->cursor];
}

char lexer_advance(Lexer *lexer) {
    char ch = lexer_now(lexer);
    if (ch == '\n') {
	lexer->line++;
	lexer->col = 1;
    } else
    lexer->col++;
    lexer->cursor++;
    return ch;
}

void lexer_lex(Lexer *lexer) {
    while (lexer_now(lexer) != EOF) {
	lexer_skip_ws(lexer);
	if (lexer_now(lexer) == EOF) break;
	if (lexer_now(lexer) != EOF &&
        (isalpha(lexer_now(lexer)) || lexer_now(lexer) == '_')) {
	    lexer_parse_word(lexer);
	    continue;
	}
	if (lexer_now(lexer) != EOF && (lexer_now(lexer) == '\"')) {
	    lexer_parse_string(lexer);
	    continue;
	}
	if (lexer_now(lexer) != EOF && isdigit(lexer_now(lexer))) {
	    lexer_parse_number(lexer);
	    continue;
	}
	int sc = lexer->col;
	switch (lexer_now(lexer)) {
	case '(':
	    lexer_advance(lexer);
	    tokenlist_add(lexer->tokens,
            make_token(TOKEN_OPEN_PAREN, "(",
            make_span(lexer->line, sc, lexer->col - 1)));
	    break;
	case ')':
	    lexer_advance(lexer);
	    tokenlist_add(lexer->tokens,
            make_token(TOKEN_CLOSE_PAREN, ")",
            make_span(lexer->line, sc, lexer->col - 1)));
	    break;
	case ';':
	    lexer_advance(lexer);
	    tokenlist_add(lexer->tokens, make_token(TOKEN_SEMI, ";", make_span(lexer->line, sc, lexer->col - 1)));
	    break;
	case '=':
 	    lexer_advance(lexer);
	    tokenlist_add(lexer->tokens, make_token(TOKEN_EQ, "=", make_span(lexer->line, sc, lexer->col - 1)));
	    break;
	case ':':
	    lexer_advance(lexer);
	    if (lexer_now(lexer) == '=') {
		lexer_advance(lexer);
		tokenlist_add(lexer->tokens, make_token(TOKEN_COLEQ, ":=", make_span(lexer->line, sc, lexer->col - 1)));
		break;
	    }
	    tokenlist_add(lexer->tokens, make_token(TOKEN_COLON, ":", make_span(lexer->line, sc, lexer->col - 1)));
	    break;
	case '.':
	    lexer_advance(lexer);
	    tokenlist_add(lexer->tokens,
            make_token(TOKEN_DOT, ".",
            make_span(lexer->line, sc, lexer->col - 1)));
	    break;
	case '+':
	    lexer_advance(lexer);
	    tokenlist_add(lexer->tokens,
            make_token(TOKEN_ADD, "+",
            make_span(lexer->line, sc, lexer->col - 1)));
	    break;
	case ',':
	    lexer_advance(lexer);
	    tokenlist_add(lexer->tokens,
            make_token(TOKEN_COMMA, ",",
            make_span(lexer->line, sc, lexer->col - 1)));
	    break;
	case '-':
	    lexer_advance(lexer);
	    tokenlist_add(lexer->tokens,
            make_token(TOKEN_SUB, "-",
            make_span(lexer->line, sc, lexer->col - 1)));
	    break;
	case '*':
	    lexer_advance(lexer);
	    tokenlist_add(lexer->tokens,
            make_token(TOKEN_MUL, "*",
            make_span(lexer->line, sc, lexer->col - 1)));
	    break;
	case '/':
	    lexer_advance(lexer);
	    tokenlist_add(lexer->tokens,
            make_token(TOKEN_DIV, "/",
            make_span(lexer->line, sc, lexer->col - 1)));
	    break;
	    default: {
		char buf[64];
		sprintf(buf, "Unknown Character: '%c'", lexer_now(lexer));
		early_fail(make_error(ERROR_FATAL, buf, 1));
	    } break;
	}
	if (lexer_now(lexer) == EOF)
	break;
    }
    tokenlist_add(lexer->tokens,
    make_token(TOKEN_EOF, "<eof>",
    make_span(lexer->line, lexer->col, lexer->col)));
}

void lexer_skip_ws(Lexer *lexer) {
    while (isspace(lexer_now(lexer))) {
	lexer_advance(lexer);
    }
}

void lexer_parse_word(Lexer *lexer) {
    std::string buffer;
    int sc = lexer->col;
    int line = lexer->line;
    while (lexer_now(lexer) != EOF &&
    (isalnum(lexer_now(lexer)) || lexer_now(lexer) == '_')) {
	// TODO: dynamic sized buffer
	buffer += lexer_advance(lexer);
    }
    Span span = make_span(line, sc, lexer->col - 1);
    if (buffer == "if") tokenlist_add(lexer->tokens, make_token(TOKEN_IF, imp_arena_strdup(arena, buffer.data()), span));
    else if (buffer == "for") tokenlist_add(lexer->tokens, make_token(TOKEN_FOR, imp_arena_strdup(arena, buffer.data()), span));
    else if (buffer == "then") tokenlist_add(lexer->tokens, make_token(TOKEN_THEN, imp_arena_strdup(arena, buffer.data()), span));
    else if (buffer == "end") tokenlist_add(lexer->tokens, make_token(TOKEN_END, imp_arena_strdup(arena, buffer.data()), span));
    else if (buffer == "else") tokenlist_add(lexer->tokens, make_token(TOKEN_ELSE, imp_arena_strdup(arena, buffer.data()), span));
    else tokenlist_add(lexer->tokens, make_token(TOKEN_IDENTIFIER, imp_arena_strdup(arena, buffer.data()), span));
}
void lexer_parse_string(Lexer *lexer) {
    lexer_advance(lexer);
    char buffer[TMP_BUFFER_SIZE] = {0};
    int inx = 0;
    int sc = lexer->col;
    int line = lexer->line;
    while (lexer_now(lexer) != '\"') {
	if (lexer_now(lexer) == '\\') {
	    lexer_advance(lexer);
	    if (lexer_now(lexer) == 'n') {
		lexer_advance(lexer);
		buffer[inx++] = '\n';
		continue;
	    }
	}
	buffer[inx++] = lexer_advance(lexer);
    }
    Span span = make_span(line, sc, lexer->col - 1);
    if (lexer_now(lexer) == '\"') {
	lexer_advance(lexer);
    }
    tokenlist_add(lexer->tokens, make_token(TOKEN_STRING, imp_arena_strdup(arena, buffer), span));
}
void lexer_parse_number(Lexer *lexer) {
    char buf[64];
    int inx = 0;
    int line = lexer->line;
    int sc   = lexer->col;
    while (lexer_now(lexer) != EOF && isdigit(lexer_now(lexer))) {
	buf[inx++] = lexer_advance(lexer);
    }
    buf[inx] = '\0';
    tokenlist_add(lexer->tokens, make_token(TOKEN_INT, imp_arena_strdup(arena, buf), make_span(line, sc, lexer->col-1)));
}
