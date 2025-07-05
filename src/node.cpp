#include "node.h"
#include "enums.h"
#include "global.h"
#include "imp_alloc.h"

AstPtr make_ident_node(std::string name, Span s) {
    auto node = std::make_unique<AstNode>(EXPR_IDENT, s);
    node->data = mk_uniq(ExprIdent, name);
    return node;
}

AstPtr make_binding_node(AstPtr lhs, AstPtr expr, bool mut, Span s) {
    auto node = std::make_unique<AstNode>(EXPR_ASSIGN, s);
    node->data = mk_uniq(ExprBinding, mut, std::move(lhs), std::move(expr));
    return node;
}

AstPtr make_call_node(AstPtr callee, AstList *args, Span s) {
    auto node = std::make_unique<AstNode>(EXPR_CALL, s);
    node->data = mk_uniq(ExprCall, std::move(callee), args);
    return node;
}

AstPtr make_field_access(AstPtr base, AstPtr field, Span s) {
    auto node = std::make_unique<AstNode>(EXPR_FIELD_ACCESS, s);
    node->data = mk_uniq(ExprFieldAccess, std::move(base), std::move(field));
    return node;
}

AstPtr make_binary_op(AstPtr lhs, AstPtr rhs, BinaryOp op, Span s) {
    auto node = std::make_unique<AstNode>(EXPR_BINOP, s);
    node->data = mk_uniq(ExprBinaryOp, std::move(lhs), std::move(rhs), op);
    return node;
}

AstPtr make_string_node(std::string string, Span s) {
    auto node = std::make_unique<AstNode>(EXPR_STRING, s);
    node->data = mk_uniq(ExprStringLit, std::move(string));
    return node;
}

AstPtr make_int_node(int value, Span s) {
    auto node = std::make_unique<AstNode>(EXPR_INT, s);
    node->data = mk_uniq(ExprIntLit, value);
    return node;
}

AstPtr make_body_node(AstList* b, Span s) {
    auto node = std::make_unique<AstNode>(EXPR_BODY, s);
    node->data = mk_uniq(ExprBody, std::move(b));
    return node;    
}

AstPtr make_if_node(AstPtr cond, AstPtr then, AstPtr else_, Span s) {
    auto node = std::make_unique<AstNode>(EXPR_IF, s);
    node->data = mk_uniq(ExprIf, std::move(cond), std::move(then), std::move(else_));
    return node;        
}

AstPtr make_brace_body_node(AstList* b, Span s);
