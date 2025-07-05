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
Value* eval_node(AstNode *node);
void _eml_runtime_error(Span s, std::string message, const char* hint);
Value *generate_value(AstNode *node);
Value *eval_call(AstNode* call, Span node_span);
Value* eval_assign(AstNode* bind);
Value* eval_field_access(AstNode* expr);
Value* eval_if(AstNode* if_expr);
Value* eval_body(AstNode* body);
