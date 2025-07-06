#pragma once

#include "./misc/containers.h"
#include "./node.h"
#include "./options.h"
#include "./types.h"
#include "enums.h"

typedef struct Runtime {
  Env *global_env;
  Env *current_env;
  StackList* stacklist;
  SourceManger* source_manager;
  StackFrame* current_frame;
} Runtime;

void interpret(Options *opt);
Error *init_runtime();
Error *init_env();
ValueRef eval_node(AstNode *node);
void _eml_runtime_error(Span s, std::string message, const char* hint);
ValueRef generate_value(AstNode *node);
ValueRef eval_call(AstNode* call, Span node_span);
ValueRef eval_assign(AstNode* bind);
ValueRef eval_field_access(AstNode* expr);
ValueRef eval_if(AstNode* if_expr);
ValueRef eval_body(AstNode* body);
