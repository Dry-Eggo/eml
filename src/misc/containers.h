#pragma once

#include "../token.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TokenList TokenList;
TokenList *make_tokenlist();
void tokenlist_add(TokenList *, Token tok);
Token tokenlist_get(TokenList *, int i);
int tokenlist_size(TokenList *);

struct AstNode;
typedef struct AstList AstList;
AstList *make_astlist();
void astlist_add(struct AstList *, struct AstNode *node);
struct AstNode *astlist_get(struct AstList *, int i);
int astlist_size(struct AstList *);

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

SourceManger* create_sourcemanager(const char* source);
const char*   sm_getline(SourceManger* sm, int line);

StackList *create_stacklist();
int stacklist_size(StackList *);
StackFrame *create_stackframe(Span span, const char *callee_name);
void push_stack_frame(StackList *stl, StackFrame *frame);
StackFrame *pop_stack_frame(StackList *stl);
StackFrame *peek_stack_frame(StackList *stl);
const char *get_stackframe_name(StackFrame *);
Span get_stackframe_span(StackFrame *);

ValueList *make_valuelist();
void valuelist_add(struct ValueList *, struct Value_ *node);
struct Value_ *valuelist_get(struct ValueList *, int i);
int valuelist_size(struct ValueList *);

Env *create_env(Env *parent);
struct Value_ *get_var(Env *e, const char *name);
void set_var(Env *e, const char *name, struct Value_ *value);
void extend_env(Env *e, Env *e2);
bool has_var(Env *e, const char *name);
Module *get_module(Env *e, const char *name);
Env *module_get_exports(Module *mod);
Module *load_module_from_file(Env *e, const char *path);
Module *load_system_module(Env *e, const char *name);

/* -------------------------------------------- */

/* ------------------ utils ------------------ */
char *readAllFile(const char* path);
bool file_exists(const char* path);
/* ------------------------------------------- */

#ifdef __cplusplus
}
#endif
