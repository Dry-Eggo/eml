#pragma once

typedef enum
{
    RUN_REPL,
    RUN_FILE,
    RUN_SHOW_HELP,
    RUN_SHOW_VERSION,
} RunOption;

typedef enum
{
    ERROR_FATAL,
    ERROR_WARNING,
    ERROR_RUNTIME,
} ErrorKind;

typedef enum
{
    // built-ins
    TOKEN_KINT,      // as a word
    TOKEN_KSTRING,

    // values
    TOKEN_INT,
    TOKEN_STRING,
    TOKEN_FLOAT,
    TOKEN_CHAR,
    TOKEN_IDENTIFIER,
    
    // keyword
    TOKEN_IF,
    TOKEN_TO,
    TOKEN_FOR,
    TOKEN_LOOP,
    TOKEN_MATCH,
    TOKEN_THEN,
    TOKEN_END,
    TOKEN_ELSE,

    // operators
    TOKEN_EQ,
    TOKEN_COLEQ, // :=
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_LT,
    TOKEN_GT,
    
    // punctuations
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_SEMI,
    TOKEN_DOT,
    TOKEN_COMMA,
    TOKEN_COLON,

    TOKEN_EOF,
} TokenKind;

typedef enum
{
    EXPR_ASSIGN,
    EXPR_CALL,
    EXPR_IDENT,
    EXPR_STRING,
    EXPR_INT,
    EXPR_FLOAT,
    EXPR_FIELD_ACCESS,
    EXPR_BINOP,
    EXPR_BRACE_BODY,
    EXPR_BODY,
    EXPR_IF,
} ExprKind;

typedef enum
{
    BINOP_ADD,
    BINOP_SUB,
    BINOP_MUL,
    BINOP_DIV,
} BinaryOp;

typedef enum
{
    VALUE_STRING,
    VALUE_INT,
    VALUE_ERROR,
    VALUE_LIST,
    VALUE_NIL,
    VALUE_NATIVEFN,
    VALUE_VARIABLE,
    VALUE_MODULE,
} ValueKind;
