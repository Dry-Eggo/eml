#include "node.h"
#include "enums.h"
#include "global.h"
#include "imp_alloc.h"

AstNode *make_ident_node(const char *name, Span s) {
  AstNode *node = imp_arena_alloc(arena, sizeof(AstNode));
  node->kind = EXPR_IDENT;
  node->identifier.identifier = name;
  node->span = s;
  return node;
}

AstNode *make_binding_node(AstNode *lhs, AstNode *expr, bool mut, Span s) {
  AstNode *node = imp_arena_alloc(arena, sizeof(AstNode));
  node->kind = EXPR_ASSIGN;
  node->binding.lhs = lhs;
  node->binding.value = expr;
  node->binding.is_mut = mut;
  node->span = s;
  return node;
}

AstNode *make_call_node(AstNode *callee, AstList *args, Span s) {
  AstNode *node = imp_arena_alloc(arena, sizeof(AstNode));
  node->kind = EXPR_CALL;
  node->call.callee = callee;
  node->call.args = args;
  node->span = s;
  return node;
}

AstNode *make_field_access(AstNode *base, AstNode *field, Span s) {
  AstNode *node = imp_arena_alloc(arena, sizeof(AstNode));
  node->kind = EXPR_FIELD_ACCESS;
  node->faccess.base = base;
  node->faccess.field = field;
  node->span = s;
  return node;
}

AstNode *make_binary_op(AstNode *lhs, AstNode *rhs, BinaryOp op, Span s) {
  AstNode *node = imp_arena_alloc(arena, sizeof(AstNode));
  node->kind = EXPR_BINOP;
  node->binop.lhs = lhs;
  node->binop.rhs = rhs;
  node->binop.op = op;
  node->span = s;
  return node;
}

AstNode *make_string_node(const char *string, Span s) {
  AstNode *node = imp_arena_alloc(arena, sizeof(AstNode));
  node->kind = EXPR_STRING;
  node->stringlit.literal = string;
  node->span = s;
  return node;
}

AstNode *make_int_node(int value, Span s) {
  AstNode *node = imp_arena_alloc(arena, sizeof(AstNode));
  node->kind = EXPR_INT;
  node->intlit.literal = value;
  node->span = s;
  return node;
}
