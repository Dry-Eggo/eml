#include "./builtins.h"
#include "global.h"
#include "misc/containers.h"
#include "types.h"
#include <stdio.h>

Value *builtin_println(Env *env, ValueList *args) {
  builtin_print(env, args);
  printf("\n");
  return value_make_nil();
}

Value *builtin_print(Env *env, ValueList *args) {
  int max = valuelist_size(args);
  for (int i = 0; i < max; i++) {
    Value *arg = valuelist_get(args, i);
    printf("%s", value_to_string(arg));
    if (i != max - 1)
      printf(" ");
  }
  return value_make_nil();
}

Value* builtin_readFile(Env* env, ValueList* args)
{
  const char* path = value_to_string(valuelist_get(args, 0));
  FILE* f = fopen(path, "r");
  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  rewind(f);
  char* content = imp_arena_alloc(arena, size+1);
  fread(content, size, 1, f);
  printf("Content: %s\n", content);
  return value_make_string(imp_arena_strdup(arena, content), make_span(1, 1, 0));
}
