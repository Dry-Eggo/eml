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
} Runtime;

void interpret(Options *opt);
Error *init_runtime();
Error *init_env();
Value* eval_node(AstNode *node);
void runtime_error(Span s, const char* message, const char* hint);
Value *generate_value(AstNode *node);
Value *eval_call(struct ExprCall call);
Value* eval_assign(struct ExprBinding bind);
Value* eval_field_access(struct ExprFieldAccess expr);
