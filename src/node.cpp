#include "./node.h"
#include "./enums.h"
#include "./defines.hpp"
#include "imp_alloc.h"
#include <format>
#include <sstream>
#include <iomanip>

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

AstPtr make_call_node(AstPtr callee, AstList args, Span s) {
    auto node = std::make_unique<AstNode>(EXPR_CALL, s);
    node->data = mk_uniq(ExprCall, std::move(callee), std::move(args));
    return node;
}

AstPtr make_nil_node (Span s) {
    auto node = std::make_unique<AstNode>(EXPR_NIL, s);
    node->data = mk_uniq(ExprNil, 0);
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

AstPtr make_body_node(AstList b, Span s) {
    auto node = std::make_unique<AstNode>(EXPR_BODY, s);
    node->data = mk_uniq(ExprBody, std::move(b));
    return node;    
}

AstPtr make_if_node(AstPtr cond, AstPtr then, AstPtr else_, Span s) {
    auto node = std::make_unique<AstNode>(EXPR_IF, s);
    node->data = mk_uniq(ExprIf, std::move(cond), std::move(then), std::move(else_));
    return node;        
}

AstPtr make_bool_node(bool v, Span s) {
    auto node = std::make_unique<AstNode>(EXPR_BOOL, s);
    node->data = mk_uniq(ExprBool, v);
    return node;
}

AstPtr make_brace_body_node(AstList b, Span s);

std::string str_node(AstNode* node, int indent) {
    std::string tab =  std::string(indent, '\t');
    std::string repr;
    if (node->kind == EXPR_INT) {
	auto &i = std::get<uniq<ExprIntLit>>(node->data);
	repr += std::format("IntLit({})", i->literal);
    } else if (node->kind == EXPR_CALL) {
	auto &c = std::get<uniq<ExprCall>>(node->data);
	auto i  = repr_node(c->callee.get());
	repr += std::format("Call<{}>(", i);
	int m = c->args.size();
	for (int i = 0; i < m; i++) {
	    auto& node = c->args.at(i);
	    auto n = repr_node(node.get());
	    repr += n;
	    if (i != m -1) {
		repr += ", ";
	    }
	}
	repr += std::format(")");
    } else if (node->kind == EXPR_IDENT) {
	auto &i = std::get<uniq<ExprIdent>>(node->data);
	repr += std::format("Identifier({})", i->identifier);
    } else if (node->kind == EXPR_BINOP) {
	auto &b = std::get<uniq<ExprBinaryOp>>(node->data);
	auto ls = str_node(b->lhs.get(), indent+1);
	auto rs = str_node(b->rhs.get(), indent+1);
	repr += std::format("BinaryExpr {{\n");
	repr += std::format("{}lhs: {}\n", tab, ls);
	repr += std::format("{}rhs: {} ", tab, rs);
	repr += std::format("}}");
    } else if (node->kind == EXPR_STRING) {
	auto &s = std::get<uniq<ExprStringLit>>(node->data);
	std::ostringstream os;
	os << std::quoted(s->literal);
	repr += std::format("StringLit({})", os.str());
    } else if (node->kind == EXPR_ASSIGN) {
	auto &r = std::get<uniq<ExprBinding>>(node->data);
	auto ls = str_node(r->lhs.get(), indent+1);
	auto rs = str_node(r->value.get(), indent+1);
	repr += std::format("Assign {{\n");
	repr += std::format("{}subject: {}\n", tab, ls);
	repr += std::format("{}value  : {} ", tab, rs);
	repr += std::format("}}");
    } else if (node->kind == EXPR_FIELD_ACCESS) {
	auto &f = std::get<uniq<ExprFieldAccess>>(node->data);
	auto bs = str_node(f->base.get(), indent+1);
	auto fs = str_node(f->field.get(), indent+1);
	repr += std::format("Field-Access {{\n");
	repr += std::format("{}base  : {}\n", tab, bs);
	repr += std::format("{}field : {} ", tab, fs);
	repr += std::format("}}");
    } else {
	repr += "<not-implemented>";
    }
    return repr;
}

std::string repr_node(AstNode* node) {
    int indent = 1;
    std::string repr;
    repr = str_node(node, indent);
    return repr;
}
