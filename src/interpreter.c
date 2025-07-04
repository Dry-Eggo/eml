#include "interpreter.h"
#include "enums.h"
#include "error.h"
#include "lexer.h"
#include "misc/containers.h"
#include "node.h"
#include "parser.h"
#include "token.h"
#include "types.h"
#include <string.h>

Runtime *runtime;
const char *source_file;
Options *options;

void interpret(Options *opt) {
    Lexer *lexer = lexer_init(opt);
    source_file = lexer->source;
    options = opt;
    Parser *parser = parser_init(opt, lexer->source, lexer->tokens);
    // Builtins
    Error *e = init_runtime();
    if (e) {
	early_fail(e);
    }
    e = init_env();
    if (e) {
	early_fail(e);
    }
    Module *core = load_system_module(runtime->global_env, "core");
    extend_env(runtime->current_env, module_get_exports(core));
    set_var(runtime->current_env, "core", value_make_module("core", core));
    for (int i = 0; i < astlist_size(parser->program); i++) {
	Value* e = eval_node(astlist_get(parser->program, i));
	if (e->kind == VALUE_ERROR)
	{
	    runtime_error(e->span, e->string_value, NULL);
	}
    }
}

Error *init_runtime() {
    Error *e = NULL;
    runtime = imp_arena_alloc(arena, sizeof(Runtime));
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

Value* eval_node(AstNode *node) {
    if (node->kind == EXPR_CALL) {
	return eval_call(node->call, node->span);
    } else if (node->kind == EXPR_ASSIGN) {
	return eval_assign(node->binding);
    } else if (node->kind == EXPR_FIELD_ACCESS) {
	return eval_field_access(node->faccess);
    } else if (node->kind == EXPR_IF) {
	return eval_if(node->if_expr);
    }
    return value_make_error("Invalid Expression", node->span);
}

Value *eval_field_access(struct ExprFieldAccess expr) {
    Value *base = generate_value(expr.base);
    if (base->kind == VALUE_MODULE) {
	Env *cur = runtime->current_env;
	runtime->current_env = module_get_exports(base->module.env);
	Value* rvalue = generate_value(expr.field);
	runtime->current_env = cur;
	return rvalue;
    }
    runtime_error(expr.base->span, "Unqualified Item", NULL);
    return value_make_nil();
}

Value *eval_assign(struct ExprBinding bind) {
    if (bind.lhs->kind == EXPR_IDENT) {
	const char *name = bind.lhs->identifier.identifier;
	Value *value = generate_value(bind.value);
	set_var(runtime->current_env, name, value_make_variable(name, value, bind.lhs->span));
    }
    return value_make_nil();
}

Value *generate_value(AstNode *node) {
    if (node->kind == EXPR_STRING) {
	return value_make_string(node->stringlit.literal, node->span);
    }
    if (node->kind == EXPR_IDENT) {
	const char *query = node->identifier.identifier;
	if (has_var(runtime->current_env, query)) {
	    Value* var =  get_var(runtime->current_env, query);
	    if (var->kind == VALUE_VARIABLE) {
		return var->vvalue.value;
	    }
	    return var;
	}
	return value_make_nil();
    }
    if (node->kind == EXPR_INT) {
	return value_make_int(node->intlit.literal, node->span);
    }
    if (node->kind == EXPR_BINOP) {
	Value *lhs = generate_value(node->binop.lhs);
	// TODO: if operator is || or &&, short circuit
	Value *rhs = generate_value(node->binop.rhs);
	BinaryOp op = node->binop.op;
	if (op == BINOP_ADD) {
	    return value_add(lhs, rhs);
	}
    }
    return eval_node(node);
}

ValueList *make_values(AstList *list) {
    ValueList *vl = make_valuelist();
    int max = astlist_size(list);
    for (int i = 0; i < max; i++) {
	AstNode *node = astlist_get(list, i);
	Value *value = generate_value(node);
	valuelist_add(vl, value);
    }
    return vl;
}

Value* eval_body(struct ExprBody body) {
    Value* lstv = value_make_nil();
    int max = astlist_size(body.body);
    for (int i = 0; i < max; ++i) {
	AstNode* node = astlist_get(body.body, i);
	lstv = generate_value(node);
    }
    return lstv;
}

Value* eval_if(struct ExprIf if_expr) {
    Value* cond = generate_value(if_expr.cond);
    if (value_istruthy(cond)) {
	return eval_body(if_expr.then_body->body);
    } else {
	return eval_body(if_expr.else_body->body);	   
    }
    return value_make_nil();
}

Value *eval_call(struct ExprCall call, Span node_span) {
    if (call.callee->kind == EXPR_IDENT) {
	const char *name = call.callee->identifier.identifier;
	if (has_var(runtime->current_env, name)) {
	    Value *fn = get_var(runtime->current_env, name);
	    if (!fn) {
		printf("Invalid Function Pointer\n");
		pop_stack_frame(runtime->stacklist);
		return value_make_nil();
	    }
	    ValueList *args = make_values(call.args);
	    if (fn->kind == VALUE_NATIVEFN) {
		StackFrame* st = create_stackframe(node_span, name);
		push_stack_frame(runtime->stacklist, st);
		StackFrame* previous_frame = runtime->current_frame;
		runtime->current_frame = st;
		if (fn->native_fn.argc != -1)
		{
		    if (valuelist_size(args) != fn->native_fn.argc)
		    {
			char buf[256];
			sprintf(buf, "Invalid Argument count. '%s' expected '%d'. got '%d'", fn->native_fn.name, fn->native_fn.argc, valuelist_size(args));
			runtime_error(call.callee->span, buf, NULL);
		    }
		}
		Value *rvalue = fn->native_fn.fn(runtime->current_env, args);
		if (rvalue->kind == VALUE_ERROR)
		{
		    runtime_error(rvalue->span, rvalue->string_value, NULL);
		}
		runtime->current_frame = previous_frame;
		pop_stack_frame(runtime->stacklist);
		return rvalue;
	    }
	}
	char msg[64];
	sprintf(msg, "Undefined Function: '%s'", name);
	runtime_error(call.callee->span, msg, NULL);
    }
    return value_make_nil();
}

void runtime_error(Span s, const char *msg, const char *hint) {
    int stack_size = stacklist_size(runtime->stacklist);
    printf("Runtime Error: %s:%d:%d: %s\n", options->infile, s.line, s.column,
    msg);
    const char *line = sm_getline(runtime->source_manager, s.line - 1);
    int st = 0;
    for (int i = 0; line[i] && i < strlen(line); i++) {
	if (i == s.column)
	break;
	st++;
    }
    printf("     %s\n", line);
    printf("    ");
    for (int i = 0; i < st; i++) {
	printf(" ");
    }
    for (int i = 0; i < strlen(line) + 4; i++) {
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
	const char *name = get_stackframe_name(sf);
	if (strcmp(name, "main") == 0) break;
	Span ss = get_stackframe_span(sf);
	const char *line = sm_getline(runtime->source_manager, ss.line - 1);

	int st = 0;
	for (int i = 0; line[i] && i < strlen(line); i++) {
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
	for (int i = 0; i < strlen(line) + 4; i++) {
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
