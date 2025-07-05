#pragma once
#include <stdbool.h>
#include "enums.h"
#include "misc/containers.h"
#include <variant>


#define uniq std::unique_ptr
#define mk_uniq(type, ...) std::make_unique<type>(__VA_ARGS__)

struct ExprBinding {
    bool	     is_mut;
    AstPtr  lhs;
    AstPtr  value;
    ExprBinding(bool im, AstPtr l, AstPtr v) : is_mut(im), lhs(std::move(l)), value(std::move(v)) {}
};

struct ExprStringLit {
    std::string literal;
    ExprStringLit(std::string l): literal(std::move(l)) { }
};

struct ExprIntLit {
    int         literal;
    ExprIntLit(int v): literal(v) {}
};


struct ExprCall {
    AstPtr callee;
    AstList*        args;
    ExprCall(AstPtr c, AstList* a): callee(std::move(c)), args(std::move(a)) {}
};

struct ExprIdent {
    std::string   identifier;
    ExprIdent(std::string name): identifier(name) {}
};

struct ExprFieldAccess {
    AstPtr  base;
    AstPtr  field;
    ExprFieldAccess(AstPtr b, AstPtr f): base(std::move(b)), field(std::move(f)) {}
};

struct ExprBinaryOp {
    AstPtr  lhs;
    AstPtr  rhs;
    BinaryOp         op;
    ExprBinaryOp(AstPtr l, AstPtr r, BinaryOp op): lhs(std::move(l)), rhs(std::move(r)), op(op) {}
};

struct ExprBraceBody {
    AstList* body;
};

// 1 |  name = {
// 2 |       int foo = do_something();
// 3 |       if foo == 69 then:
// 4 |            return "Dry";
// 5 |       else:    
// 6 |            return "Eggo";
// 7 |       end
// 8 |  };
// 9 | 
struct ExprBody {
    AstList* body;
    ExprBody(AstList* b): body(std::move(b)) {}
};

struct ExprIf {
    AstPtr  cond;
    AstPtr then_body;
    AstPtr else_body;
    ExprIf(AstPtr c, AstPtr t, AstPtr e): cond(std::move(c)), then_body(std::move(t)), else_body(std::move(e)) {}
};

struct ExprNop {
    ExprNop(int i) {}
};

typedef struct AstNode {
    ExprKind kind;
    Span     span;
    std::variant<
     uniq<ExprBinding>, uniq<ExprCall>, uniq<ExprStringLit>, uniq<ExprIntLit>, uniq<ExprIdent>, uniq<ExprFieldAccess>, uniq<ExprBinaryOp>, uniq<ExprIf>, uniq<ExprBody>, uniq<ExprNop>, uniq<ExprBraceBody>>
    data;
    AstNode(ExprKind kind, Span s): kind(kind), span(s), data(mk_uniq(ExprNop, 0)) {}	 
} AstNode;

AstPtr make_binding_node(AstPtr , AstPtr expr, bool mut, Span s);
AstPtr make_call_node   (AstPtr callee,  AstList   * args, Span s);
AstPtr make_field_access(AstPtr base,  AstPtr field ,    Span s);
AstPtr make_binary_op   (AstPtr lhs,  AstPtr rhs, BinaryOp op, Span s);
AstPtr make_string_node (std::string string, Span s);
AstPtr make_int_node    (int         value, Span s);
AstPtr make_ident_node  (std::string name, Span s);
AstPtr make_body_node   (AstList* b, Span s);
AstPtr make_brace_body_node  (AstList* b, Span s);
AstPtr make_if_node(AstPtr cond, AstPtr then, AstPtr else_, Span s);
