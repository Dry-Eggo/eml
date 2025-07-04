#include "containers.h"

#include "../builtins.h"
#include "../global.h"
#include "../types.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct TokenList {
  vector<Token> tokens;
};

TokenList *make_tokenlist() {
  TokenList *tl = (TokenList *)imp_arena_alloc(arena, sizeof(TokenList));
  return tl;
}

void tokenlist_add(TokenList *t, Token tok) { t->tokens.push_back(tok); }

Token tokenlist_get(TokenList *t, int n) { return t->tokens.at(n); }

int tokenlist_size(TokenList *t) { return t->tokens.size(); }

char *readAllFile(const char *path) {
  fstream file = fstream(path);
  stringstream s;
  s << file.rdbuf();
  string content = s.str();
  return imp_arena_strdup(arena, content.data());
}

struct AstList {
  vector<struct AstNode *> nodes;
};

AstList *make_astlist() {
  AstList *tl = (AstList *)imp_arena_alloc(arena, sizeof(AstList));
  return tl;
}

void astlist_add(AstList *t, struct AstNode *node) { t->nodes.push_back(node); }

struct AstNode *astlist_get(AstList *t, int n) { return t->nodes.at(n); }

int astlist_size(AstList *t) { return t->nodes.size(); }

struct ValueList {
  vector<struct Value_ *> values;
};

ValueList *make_valuelist() {
  ValueList *tl = (ValueList *)imp_arena_alloc(arena, sizeof(ValueList));
  return tl;
}

void valuelist_add(ValueList *t, struct Value_ *value) {
  t->values.push_back(value);
}

struct Value_ *valuelist_get(ValueList *t, int n) { return t->values.at(n); }

int valuelist_size(ValueList *t) { return t->values.size(); }

// Runtime

struct Module {
  const char *name;
  unordered_map<string, Value_ *> exports;
};

Module *create_module(const char *name) {
  void *mem = imp_arena_alloc(arena, sizeof(Module));
  return new (mem) Module();
}

struct Env {
  Env *parent;
  unordered_map<string, Value_ *> vars;
  unordered_map<string, Module *> modules;
};

Env *create_env(Env *parent) {
  void *mem = imp_arena_alloc(arena, sizeof(Env));
  if (!mem) {
    return NULL;
  }
  Env *env = new (mem) Env();
  env->parent = parent;
  return env;
}

bool has_var(Env *e, const char *name) {
  auto it = e->vars.find(name);
  return (it != e->vars.end());
}

Value_ *get_var(Env *e, const char *name) {
  // Todo: Check
  return e->vars[name];
}
void set_var(Env *e, const char *name, Value_ *value) { e->vars[name] = value; }

void extend_env(Env *e, Env *e2) {
  for (const auto &pair : e2->vars) {
    e->vars.insert(pair);
  }
}

Env *module_get_exports(Module *mod) {
  Env *e = create_env(NULL);
  e->vars = mod->exports;
  return e;
}

Module *load_system_module(Env *env, const char *name) {
  Module *mod = create_module(name);
  if (strcmp(name, "core") == 0) {
    mod->name = "core";
    mod->exports["println"] =
        value_make_native_fn("println", -1, builtin_println);
    mod->exports["print"] = value_make_native_fn("print", -1, builtin_print);
    mod->exports["readFile"] = value_make_native_fn("readFile", 1, builtin_readFile);
  }
  env->modules[name] = mod;
  return mod;
}

struct ModuleTable {
  unordered_map<string, Module *> modules;
};

struct StackFrame {
  Span callee_span;
  const char *callee_name;
};

struct StackList {
  vector<StackFrame *> stackframes;
};

StackList *create_stacklist() {
  void *mem = imp_arena_alloc(arena, sizeof(StackList));
  return new (mem) StackList();
}

int stacklist_size(StackList *stl) { return stl->stackframes.size(); }

StackFrame *create_stackframe(Span span, const char *callee_name) {
  StackFrame *f = (StackFrame *)imp_arena_alloc(arena, sizeof(StackFrame));
  f->callee_name = callee_name;
  f->callee_span = span;
  return f;
}
void push_stack_frame(StackList *stl, StackFrame *frame) {
  stl->stackframes.push_back(frame);
}
StackFrame *pop_stack_frame(StackList *stl) {
  StackFrame *s = stl->stackframes.back();
  stl->stackframes.pop_back();
  return s;
}
const char *get_stackframe_name(StackFrame *s) { return s->callee_name; }
Span get_stackframe_span(StackFrame *s) { return s->callee_span; }

struct SourceManger {
  vector<string> lines;
  SourceManger(string source) {
    stringstream s;
    s << source;
    string line;
    while (getline(s, line)) {
      lines.push_back(line);
    }
  }
};

SourceManger *create_sourcemanager(const char *source) {
  void *mem = imp_arena_alloc(arena, sizeof(SourceManger));
  return new (mem) SourceManger(source);
}
const char *sm_getline(SourceManger *sm, int line) {
  return imp_arena_strdup(arena, sm->lines.at(line).data());
}
