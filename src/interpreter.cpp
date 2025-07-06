#include "interpreter.hpp"
#include "enums.h"
#include "error.h"
#include "lexer.h"
#include "misc/containers.h"
#include "node.h"
#include "parser.h"
#include "token.h"
#include "types.h"
#include <string.h>
#include <format>

#define vu(type) std::unique_ptr<type>

Runtime *runtime;
std::string source_file;
Options *options;

void interpret(Options *opt) {
    Error *e = init_runtime();
    if (e) {
	early_fail(e);
    }
    e = init_env();
    if (e) {
	early_fail(e);
    }
    
    Module *core = load_system_module(runtime->global_env, "core");
    set_var(module_get_exports(core), "__prompt__", value_make_string(">>", make_span(0, 0, 0)));
    extend_env(runtime->current_env, module_get_exports(core));
    set_var(runtime->current_env, "core", value_make_module("core", core));
    if (opt->run_option == RUN_FILE) {
	Lexer *lexer = lexer_init(opt);
	source_file = lexer->source;
	options = opt;
	Parser *parser = parser_init(opt, lexer->source, lexer->tokens);
	if (opt->show_ast) {
	    printf("---------------------------------------------\n");
	    for (int i = 0; i < parser->program.size(); i++) {
		auto node = parser->program.at(i).get();
		std::cout << "\n" << repr_node(node) << "\n";
	    }
	    printf("---------------------------------------------\n");
	    exit(0);
	}
	for (int i = 0; i < parser->program.size(); i++) {
	    auto node = parser->program.at(i).get();
	    ValueRef e = generate_value(node);
	    if (e->kind == VALUE_ERROR) {
		auto error_msg = std::get<std::string>(e->data);
		_eml_runtime_error(e->span, error_msg.c_str(), NULL);
	    }
	}
    } else if (opt->run_option == RUN_REPL) {
	auto lexer = lexer_init_repl(opt);
	auto prompt = get_var(module_get_exports(core), "__prompt__");
	if (!prompt) {
	    printf("Invalid Prompt\n");
	    exit(0);
	}
	while (true) {
	    printf("Here\n");	    
	    std::cout << value_to_string(prompt) << " ";
	    std::string line;
	    std::getline(cin, line);
	    set_source(lexer, line);
	    auto tlist = lexer_lex_line(lexer);
	    std::cout << "Lexed: " << tokenlist_size(tlist) << "\n";
	}
    }
}

Error *init_runtime() {
    Error *e = NULL;
    runtime = (Runtime *)imp_arena_alloc(arena, sizeof(Runtime));
    if (!runtime) {
	return make_error(ERROR_FATAL, "Unable to start runtime", 1);
    }
    runtime->stacklist = create_stacklist();
    push_stack_frame(runtime->stacklist,
    create_stackframe(make_span(1, 1, 1), "main"));
    runtime->source_manager = create_sourcemanager(source_file);
    e = init_env();
    return e;
}

Error *init_env() {
    runtime->global_env = create_env(NULL);
    if (!runtime->global_env) {
	return make_error(ERROR_FATAL, "Unable to initialize global environment",
        1);
    }
    runtime->current_env = runtime->global_env;
    return NULL;
}

ValueRef eval_node(AstNode *node) {
    if (node->kind == EXPR_CALL) {
	return eval_call(node, node->span);
    } else if (node->kind == EXPR_ASSIGN) {
	return eval_assign(node);
    } else if (node->kind == EXPR_FIELD_ACCESS) {
	return eval_field_access(node);
    } else if (node->kind == EXPR_IF) {
	return eval_if(node);
    }
    return value_make_error("Invalid Expression", node->span);
}

ValueRef eval_field_access(AstNode *expr) {
    auto &field_ = std::get<vu(ExprFieldAccess)>(expr->data);
    auto &field = field_->field;
    auto &base_ = field_->base;
    ValueRef base = generate_value(field_->base.get());

    if (base->kind == VALUE_MODULE) {
	Env *cur = runtime->current_env;
	if (!runtime->current_env) {
	    printf("Environment Corrupted\n");
	}
	auto mod = as(ModuleValue, base);
	runtime->current_env = module_get_exports(mod.env);
	if (!runtime->current_env) {
	    printf("Environment Module\n");
	}
	if (!field) {
	    printf("Invalid field\n");
	}
	ValueRef rvalue = generate_value(field.get());
	runtime->current_env = cur;
	return rvalue;
    }
    _eml_runtime_error(base->span, "Unqualified Item", NULL);
    return value_make_nil();
}

ValueRef eval_assign(AstNode *bind) {
    auto &assign = std::get<vu(ExprBinding)>(bind->data);
    if (assign->lhs->kind == EXPR_IDENT) {
	auto &identifier = std::get<vu(ExprIdent)>(assign->lhs->data);
	auto name = identifier->identifier;
	ValueRef value = generate_value(assign->value.get());
	bool is_const = (!assign->is_mut);
	if (has_var(runtime->current_env, name.c_str())) {
	    auto var = get_var(runtime->current_env, name.c_str());
	    if (var->kind == VALUE_VARIABLE) {
		auto vvalue = as(VariableValue, var);
		if (vvalue.is_const) {
		    auto message = std::format("'{}' was declared as constant", name);
		    _eml_runtime_error(bind->span, message.c_str(), NULL);
		}
	    }
	}
	set_var(runtime->current_env, imp_arena_strdup(arena, name.data()), value_make_variable(name, value, is_const, assign->lhs->span));
    }
    return value_make_nil();
}

ValueRef generate_value(AstNode *node) {
    if (node->kind == EXPR_STRING) {
	auto &string_node = std::get<vu(ExprStringLit)>(node->data);
	return value_make_string(imp_arena_strdup(arena, string_node->literal.data()), node->span);
    }

    if (node->kind == EXPR_BOOL) {
	auto &bool_node = std::get<vu(ExprBool)>(node->data);
	return value_make_bool(bool_node->value, node->span);
    }
    
    if (node->kind == EXPR_IDENT) {
	auto &ident_node = std::get<vu(ExprIdent)>(node->data);
	auto query = ident_node->identifier;
	if (has_var(runtime->current_env, query.c_str())) {
	    ValueRef var = get_var(runtime->current_env, query.c_str());
	    if (var->kind == VALUE_VARIABLE) {
		auto vvalue = as(VariableValue, var);
		return vvalue.value;
	    }
	    return var;
	}
	return value_make_nil();
    }
    if (node->kind == EXPR_INT) {
	auto &int_node = std::get<vu(ExprIntLit)>(node->data);
	return value_make_int(int_node->literal, node->span);
    }
    if (node->kind == EXPR_NIL) {
	return value_make_rnil(node->span);
    }
    if (node->kind == EXPR_BINOP) {
	auto &binop = std::get<vu(ExprBinaryOp)>(node->data);
	ValueRef lhs = generate_value(binop->lhs.get());
	// TODO: if operator is || or &&, short circuit
	ValueRef rhs = generate_value(binop->rhs.get());
	BinaryOp op = binop->op;
	if (op == BINOP_ADD) {
	    return value_add(lhs, rhs);
	} if (op == BINOP_EQ) {
	    auto r = value_eq(lhs, rhs);
	    if (r->kind == VALUE_ERROR) {
		auto err = as(ErrorValue, r);
		_eml_runtime_error(node->span, err.message.c_str(), NULL);
	    }
	    return r;
	}
    }
    return eval_node(node);
}

ValueList *make_values(AstList &list) {
    ValueList *vl = make_valuelist();
    int max = list.size();
    for (int i = 0; i < max; i++) {
	auto node = list.at(i).get();
	ValueRef value = generate_value(node);
	valuelist_add(vl, value);
    }
    return vl;
}

ValueRef eval_body(AstNode *body_) {
    ValueRef lstv = value_make_nil();
    auto &body = std::get<vu(ExprBody)>(body_->data);
    if (body) {
	int max = body->body.size();
	for (int i = 0; i < max; ++i) {
	    auto &n = body->body.at(i);
	    if (!n) printf("Invalid NodePtr\n");
	    auto node = n.get();
	    if (!node) {
		printf("Invalid Node\n");
		continue;
	    }
	    lstv = generate_value(node);
	}
    }
    return lstv;
}

ValueRef eval_if(AstNode *node) {
    auto &if_expr = std::get<vu(ExprIf)>(node->data);
    ValueRef cond = generate_value(if_expr->cond.get());
    if (value_istruthy(cond)) {
	if (if_expr->then_body != NULL)
	return eval_body(if_expr->then_body.get());
    } else {
	if (if_expr->else_body)
	return eval_body(if_expr->else_body.get());
    }
    return value_make_nil();
}

ValueRef eval_call(AstNode *node, Span node_span) {
    auto &call = std::get<vu(ExprCall)>(node->data);
    if (call->callee->kind == EXPR_IDENT) {
	auto &identifier = std::get<vu(ExprIdent)>(call->callee->data);
	auto name = identifier->identifier;
	if (has_var(runtime->current_env, name.c_str())) {
	    ValueRef fn = get_var(runtime->current_env, name.c_str());
	    if (!fn) {
		printf("Invalid Function Pointer\n");
		pop_stack_frame(runtime->stacklist);
		return value_make_nil();
	    }
	    ValueList *args = make_values(call->args);
	    if (fn->kind == VALUE_NATIVEFN) {
		StackFrame *st = create_stackframe(node_span, name.c_str());
		push_stack_frame(runtime->stacklist, st);
		StackFrame *previous_frame = runtime->current_frame;
		runtime->current_frame = st;
		auto func = as(NativeFn, fn);
		if (func.argc != -1) {
		    if (valuelist_size(args) != func.argc) {
			char buf[256];
			sprintf(buf, "Invalid Argument count. '%s' expected '%d'. got '%d'",
			func.name.c_str(), func.argc,
			valuelist_size(args));
			_eml_runtime_error(call->callee->span, buf, NULL);
		    }
		}
		ValueRef rvalue = func.fn(runtime->current_env, args);
		if (rvalue->kind == VALUE_ERROR) {
		    auto error_msg = as(std::string, rvalue);
		    _eml_runtime_error(rvalue->span, error_msg.c_str(), NULL);
		}
		runtime->current_frame = previous_frame;
		pop_stack_frame(runtime->stacklist);
		return rvalue;
	    }
	}
	char msg[64];
	sprintf(msg, "Undefined Function: '%s'", name.c_str());
	_eml_runtime_error(call->callee->span, msg, NULL);
    }
    return value_make_nil();
}

void _eml_runtime_error(Span s, std::string msg, const char* hint) {
    int stack_size = stacklist_size(runtime->stacklist);
    printf("Runtime Error: %s:%d:%d: %s\n", options->infile, s.line, s.column,
    msg.c_str());
    auto line = sm_getline(runtime->source_manager, s.line - 1);
    int st = 0;
    for (int i = 0; line[i] && i < (int)strlen(line); i++) {
	if (i == s.column)
	break;
	st++;
    }
    printf("     %s\n", line);
    printf("    ");
    for (int i = 0; i < st; i++) {
	printf(" ");
    }
    for (int i = 0; i < (int)strlen(line) + 4; i++) {
	if (i >= s.column) {
	    while (i <= s.colend) {
		printf("^");
		i++;
	    }
	}
    }
    printf("\n");
    for (int i = 0; i < stack_size; i++) {
	StackFrame *sf = pop_stack_frame(runtime->stacklist);
	auto name = get_stackframe_name(sf);
	if (strcmp(name, "main") == 0)
	break;
	Span ss = get_stackframe_span(sf);
	auto line = sm_getline(runtime->source_manager, ss.line - 1);

	int st = 0;
	for (int i = 0; line[i] && i < (int)strlen(line); i++) {
	    if (i == ss.column)
            break;
	    st++;
	}

	printf("In: %s (%d:%d):\n", name, ss.line, ss.column);
	printf("     %s\n", line);
	printf("    ");
	for (int i = 0; i < st; i++) {
	    printf(" ");
	}
	for (int i = 0; i < (int)strlen(line) + 4; i++) {
	    if (i >= ss.column) {
		while (i <= ss.colend) {
		    printf("^");
		    i++;
		}
	    }
	}
	printf("\n");
    }
    exit(1);
}
