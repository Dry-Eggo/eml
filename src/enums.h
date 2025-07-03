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

    // operators
    TOKEN_EQ,
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
} ExprKind;

typedef enum
{
    VALUE_STRING,
    VALUE_INT,
    VALUE_ERROR,
    VALUE_LIST,
} ValueKind;
