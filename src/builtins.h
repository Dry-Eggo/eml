#pragma once
#include "misc/containers.h"
#include "types.h"

Value *builtin_println(Env *env, ValueList *args);
Value *builtin_print(Env *env, ValueList *args);
Value *builtin_readFile(Env *env, ValueList *args);

