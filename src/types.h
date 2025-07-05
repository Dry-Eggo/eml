#pragma once

#include "./misc/containers.h"
#include "enums.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NativeFn_ {
    std::string name;
    int argc;
    struct Value_ *(*fn)(Env *env, ValueList *args);
} NativeFn;

typedef struct ModuleValue {
    std::string name;
    Module*  env;
} ModuleValue ;

typedef struct ErrorValue {
    Span        error_span;
    std::string message;
} ErrorValue;

typedef struct VariableValue {
    std::string  name;
    struct Value_* value;
} VariableValue;

typedef struct Value_ {
    ValueKind kind;
    Span span;
    union {
	std::string string_value;
	int int_value;
	struct Value_ *ref_value;
	struct NativeFn_ native_fn;
	struct ModuleValue module;
	struct ErrorValue  err;
	struct VariableValue vvalue;
    };
} Value;

std::string value_to_string(Value *v);
Value *value_make_int(int v, Span s);
Value *value_make_string(std::string s, Span sp);
Value *value_make_error(std::string msg, Span error_span);
Value *value_make_ref(Value *v);
Value *value_make_nil();
Value *value_make_module(std::string mod_name, Module* mod);
Value *value_make_native_fn(std::string name, int argc, Value *(*fn)(Env *env, ValueList *args));
Value *value_make_variable(std::string name, Value* value, Span sp);
Value *value_add(Value *v1, Value *v2);
bool  value_istruthy(Value* v);
#ifdef __cplusplus
}
#endif
