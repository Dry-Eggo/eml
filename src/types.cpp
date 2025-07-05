#include "./types.h"
#include "./global.h"
#include "enums.h"

Value *value_make_int(int v, Span s) {
  Value *val = (Value *)imp_arena_alloc(arena, sizeof(Value));
  val->kind = VALUE_INT;
  val->int_value = v;
  val->span = s;
  return val;
}

Value *value_make_string(std::string s, Span sp) {
  Value *val = (Value *)imp_arena_alloc(arena, sizeof(Value));
  val->kind = VALUE_STRING;
  val->string_value = imp_arena_strdup(arena, s.data());
  val->span = sp;
  return val;
}

Value *value_make_variable(std::string name, Value *value, bool const_,  Span sp) {
  Value *val = (Value *)imp_arena_alloc(arena, sizeof(Value));
  val->kind = VALUE_VARIABLE;
  val->vvalue.name = name;
  val->vvalue.value = value;
  val->vvalue.is_const = const_;
  val->span = sp;
  return val;
}

Value *value_make_nil() {
  static Value *null_value = NULL;
  if (!null_value) {
    null_value = (Value *)imp_arena_alloc(arena, sizeof(Value));
  }
  return null_value;
}

Value *value_make_native_fn(std::string name, int argc,
                            Value *(*fn)(Env *env, ValueList *args)) {
  Value *val = (Value *)imp_arena_alloc(arena, sizeof(Value));
  val->kind = VALUE_NATIVEFN;
  val->native_fn.fn = fn;
  val->native_fn.argc = argc;
  val->native_fn.name = name;
  return val;
}

Value *value_make_module(std::string mod_name, Module *mod) {
  Value *val = (Value *)imp_arena_alloc(arena, sizeof(Value));
  val->kind = VALUE_MODULE;
  val->module.env = mod;
  val->module.name = mod_name;
  return val;
}

Value *value_make_error(std::string msg, Span error_span) {
  Value *val = (Value *)imp_arena_alloc(arena, sizeof(Value));
  val->kind = VALUE_ERROR;
  val->string_value = msg;
  val->span = error_span;
  return val;
}

Value *value_add(Value *v1, Value *v2) {
  if (v1->kind == VALUE_INT && v2->kind == VALUE_INT) {
    return value_make_int(v1->int_value + v2->int_value,
                          merge_span(v1->span, v2->span));
  }
  // TODO: replace with error value after integrating span in value struct
  return value_make_nil();
}

bool value_istruthy(Value *v) {
  if (v->kind == VALUE_INT) {
    if (v->int_value > 0) {
      return true;
    }
  }
  return false;
}

std::string value_to_string(Value *v) {
  if (v->kind == VALUE_STRING) {
    return v->string_value;
  }
  if (v->kind == VALUE_INT) {
    char buf[64];
    sprintf(buf, "%d", v->int_value);
    return imp_arena_strdup(arena, buf);
  }
  if (v->kind == VALUE_VARIABLE) {
    return value_to_string(v->vvalue.value);
  }
  return "nil";
}
