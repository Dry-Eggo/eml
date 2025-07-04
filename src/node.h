#pragma once
#include <stdbool.h>
#include "enums.h"
#include "misc/containers.h"

struct ExprBinding
{
    bool	     is_mut;
    struct AstNode*  lhs;
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

struct ExprFieldAccess
{
    struct AstNode*  base;
    struct AstNode*  field;
};

struct ExprBinaryOp
{
    struct AstNode*  lhs;
    struct AstNode*  rhs;
    BinaryOp         op;
};

typedef struct AstNode
{
    ExprKind kind;
    Span     span;
    union
    {
       	struct ExprBinding     binding;
       	struct ExprCall        call;
       	struct ExprStringLit   stringlit;
       	struct ExprIntLit      intlit;
       	struct ExprIdent       identifier;
       	struct ExprFieldAccess faccess;
       	struct ExprBinaryOp    binop;
    };
} AstNode;

AstNode* make_binding_node(AstNode* , AstNode* expr, bool mut, Span s);
AstNode* make_call_node   (AstNode* callee,  AstList   * args, Span s);
AstNode* make_field_access(AstNode* base,  AstNode* field ,    Span s);
AstNode* make_binary_op   (AstNode* lhs,  AstNode* rhs, BinaryOp op, Span s);
AstNode* make_string_node (const char* string, Span s);
AstNode* make_int_node    (int         value, Span s);
AstNode* make_ident_node  (const char* name, Span s);
