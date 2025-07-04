#pragma once
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#define IMP_ARENA_ALIGNMENT (sizeof(void *))

typedef struct {
  char *buffer;
  size_t capacity;
  size_t cursor;
} ImpArena;

#ifdef __cplusplus
extern "C" {
#endif
    
ImpArena *imp_arena_init(size_t capacity);
void *imp_arena_alloc(ImpArena *arena, size_t size);
char *imp_arena_strdup(ImpArena* arena, const char* str);
void imp_arena_reset(ImpArena *arena);
void imp_arena_free(ImpArena *arena);

#ifdef __cplusplus
}
#endif
