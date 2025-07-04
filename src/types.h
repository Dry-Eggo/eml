#pragma once

#include "./misc/containers.h"
#include "enums.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NativeFn_ {
  const char *name;
  int argc;
  struct Value_ *(*fn)(Env *env, ValueList *args);
} NativeFn;

typedef struct ModuleValue
{
  const char* name;
  Module*  env;
} ModuleValue ;

typedef struct Value_ {
  ValueKind kind;
  Span span;
  union {
    const char *string_value;
    int int_value;
    struct Value_ *ref_value;
    struct NativeFn_ native_fn;
    struct ModuleValue module;
  };
} Value;

const char *value_to_string(Value *v);
Value *value_make_int(int v, Span s);
Value *value_make_string(const char *s, Span sp);
Value *value_make_error(const char* msg, Span error_span);
Value *value_make_ref(Value *v);
Value *value_make_nil();
Value *value_make_module(const char* mod_name, Module* mod);
Value *value_make_native_fn(const char *name, int argc,
                            Value *(*fn)(Env *env, ValueList *args));
Value *value_add(Value *v1, Value *v2);
#ifdef __cplusplus
}
#endif
