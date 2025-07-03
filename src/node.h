#pragma once
#include <stdbool.h>
#include "misc/containers.h"

struct ExprBinding
{
    bool	     is_mut;
    const char*      name;
    struct AstNode*  value;
};

struct ExprStringLit
{
    const char* literal;
};

struct ExprIntLit
{
    int         literal;
};


struct ExprCall
{
    struct AstNode* callee;
    AstList*        args;
};

struct ExprIdent
{
    const char*   identifier;
};

typedef struct AstNode
{
    ExprKind kind;
    Span     span;
    union
    {
	struct ExprBinding   binding;
	struct ExprCall      call;
	struct ExprStringLit stringlit;
	struct ExprIntLit    intlit;
	struct ExprIdent     identifier;
    };
} AstNode;

AstNode* make_binding_node(const char* name, AstNode* expr, bool mut);
AstNode* make_call_node   (AstNode* callee,  AstList   * args);
AstNode* make_string_node (const char* string);
AstNode* make_int_node    (int         value);
AstNode* make_ident_node  (const char* name);
