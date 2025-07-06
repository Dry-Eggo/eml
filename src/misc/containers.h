#pragma once

#include "../token.h"
#include <stdbool.h>
#include <string>
#include <memory>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

typedef struct TokenList TokenList;
TokenList *make_tokenlist();
void tokenlist_add(TokenList *, Token tok);
Token tokenlist_get(TokenList *, int i);
int tokenlist_size(TokenList *);

struct AstNode;
using AstPtr = std::unique_ptr<AstNode>;


/* ------------------ Runtime ----------------- */

typedef struct Env Env;
typedef struct Vm Vm;
typedef struct Module Module;
typedef struct ModuleTable ModuleTable;
struct Value_;
typedef struct ValueList ValueList;
typedef struct StackFrame StackFrame;
typedef struct StackList StackList;
typedef struct SourceManger SourceManger;

SourceManger* create_sourcemanager(std::string source);
const char*   sm_getline(SourceManger* sm, int line);

StackList *create_stacklist();
int stacklist_size(StackList *);
StackFrame *create_stackframe(Span span, const char *callee_name);
void push_stack_frame(StackList *stl, StackFrame *frame);
StackFrame *pop_stack_frame(StackList *stl);
StackFrame *peek_stack_frame(StackList *stl);
const char *get_stackframe_name(StackFrame *);
Span get_stackframe_span(StackFrame *);

using ValueRef = std::shared_ptr<Value_>;

ValueList *make_valuelist();
void valuelist_add(struct ValueList *, ValueRef node);
ValueRef valuelist_get(struct ValueList *, int i);
int valuelist_size(struct ValueList *);

Env *create_env(Env *parent);
ValueRef get_var(Env *e, const char *name);
void set_var(Env *e, const char *name, ValueRef value);
void extend_env(Env *e, Env *e2);
bool has_var(Env *e, const char *name);
sh(Module) get_module(Env *e, const char *name);
Env *module_get_exports(Module *mod);
Module *load_module_from_file(Env *e, const char *path);
Module *load_system_module(Env *e, const char *name);

/* -------------------------------------------- */

/* ------------------ utils ------------------ */
char *readAllFile(const char* path);
bool file_exists(const char* path);
/* ------------------------------------------- */

