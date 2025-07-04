#pragma once
#include "misc/containers.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

Value *builtin_println(Env *env, ValueList *args);
Value *builtin_print(Env *env, ValueList *args);
Value *builtin_readFile(Env *env, ValueList *args);

#ifdef __cplusplus
}
#endif
