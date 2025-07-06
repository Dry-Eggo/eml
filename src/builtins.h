#pragma once
#include "misc/containers.h"
#include "types.h"

ValueRef builtin_println(Env *env, ValueList *args);
ValueRef builtin_print(Env *env, ValueList *args);
ValueRef builtin_readFile(Env *env, ValueList *args);

