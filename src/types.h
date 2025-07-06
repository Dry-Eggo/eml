#pragma once

#include "./misc/containers.h"
#include "enums.h"
#include <variant>

#define mk_shr(type, ...) std::make_shared<type>(__VA_ARGS__)
#define sh(type)          std::shared_ptr<type>

using ValueRef = sh(Value_);
using NativeSig =  ValueRef(*)(Env *env, ValueList *args);
typedef struct NativeFn_ {
    std::string name;
    int argc;
    NativeSig fn;
    NativeFn_(std::string n, int a, NativeSig f): name(std::move(n)), argc(a) {
	fn = f;
    }
} NativeFn;

typedef struct ModuleValue {
    std::string name;
    sh(Module)  env;
    ModuleValue(std::string n, Module* e): name(std::move(n)) {
	env = mk_shr(Module, e);
    }
} ModuleValue ;

typedef struct ErrorValue {
    Span        error_span;
    std::string message;
    ErrorValue(Span s, std::string m): error_span(s), message(std::move(m)) {}
} ErrorValue;

typedef struct VariableValue {
    std::string  name;
    bool is_const;
    ValueRef value;
    VariableValue(std::string n, bool ic, ValueRef r): name(std::move(n)), is_const(ic), value(r) {}
} VariableValue;

typedef struct NilValue {
    NilValue(int i) {}
} NilValue;

using ValueRef = std::shared_ptr<Value_>;
using ValueData = std::variant<
        std::string,
	int,
	bool,
	ValueRef,
	NativeFn,
	ModuleValue,
	ErrorValue ,
	VariableValue,
	NilValue
    >;

typedef struct Value_ {
    ValueKind kind;
    Span span;
    ValueData data;
    Value_(Span s): data(NilValue(0)), span(s), kind(VALUE_NIL) {}
    Value_(Span s, ValueKind k, ValueData d): kind(k), span(s), data(d) {}
} Value;

std::string value_to_string(ValueRef v);
ValueRef value_make_int(int v, Span s);
ValueRef value_make_string(std::string s, Span sp);
ValueRef value_make_bool(bool v, Span s);
ValueRef value_make_error(std::string msg, Span error_span);
ValueRef value_make_ref(ValueRef v);
ValueRef value_make_nil();
ValueRef value_make_rnil(Span s);
ValueRef value_make_module(std::string mod_name, Module* mod);
ValueRef value_make_native_fn(std::string name, int argc, ValueRef (*fn)(Env *env, ValueList *args));
ValueRef value_make_variable(std::string name, ValueRef value, bool const_, Span sp);
ValueRef value_add(ValueRef v1, ValueRef v2);
ValueRef value_eq(ValueRef v1, ValueRef v2);
bool  value_istruthy(ValueRef v);
