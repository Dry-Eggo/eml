#include "./builtins.h"
#include "global.h"
#include "misc/containers.h"
#include "types.h"
#include <stdio.h>

#include <iostream>
#include <stdlib.h>

ValueRef builtin_println(Env *env, ValueList *args) {
  builtin_print(env, args);
  printf("\n");
  return value_make_nil();
}

ValueRef builtin_print(Env *env, ValueList *args) {
  int max = valuelist_size(args);
  for (int i = 0; i < max; i++) {
    ValueRef arg = valuelist_get(args, i);
    std::cout << value_to_string(arg);
    if (i != max - 1)
      printf(" ");
  }
  fflush(stdout);
  return value_make_nil();
}

ValueRef builtin_readFile(Env *env, ValueList *args) {
  ValueRef path_value = valuelist_get(args, 0);
  auto path_string = value_to_string(path_value);
  if (!file_exists(path_string.c_str())) {
    return value_make_error("Path Does Not Exist", path_value->span);
  }
  const char *content = readAllFile(path_string.c_str());
  return value_make_string(content,
                           get_stackframe_span(runtime->current_frame));
}
