#include "node.h"
#include "imp_alloc.h"
#include "global.h"

AstNode* make_ident_node(const char* name)
{
    AstNode* node         =  imp_arena_alloc(arena, sizeof(AstNode));
    node->kind            =  EXPR_IDENT;
    node->identifier.identifier =  name;
    return node;
}

AstNode* make_binding_node(const char* name, AstNode* expr, bool mut)
{
    AstNode* node         =  imp_arena_alloc(arena, sizeof(AstNode));
    node->kind            =  EXPR_ASSIGN;
    node->binding.name    =  name;
    node->binding.value   =  expr;
    node->binding.is_mut  =  mut;
    return node;
}

AstNode* make_call_node   (AstNode* callee,  AstList   * args)
{
    AstNode* node         =  imp_arena_alloc(arena, sizeof(AstNode));
    node->kind            =  EXPR_CALL;
    node->call.callee     =  callee;
    node->call.args       =  args;
    return node;
}

AstNode* make_string_node (const char* string)
{
    AstNode* node           =  imp_arena_alloc(arena, sizeof(AstNode));
    node->kind              =  EXPR_STRING;
    node->stringlit.literal = string;
    return node;
}

AstNode* make_int_node    (int         value);
