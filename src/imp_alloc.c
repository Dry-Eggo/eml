#include "imp_alloc.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t align_up(size_t v, size_t alignment) {
  return (v + alignment - 1) & ~(alignment - 1);
}

ImpArena *imp_arena_init(size_t capacity) {
  ImpArena *arena = (ImpArena *)malloc(sizeof(ImpArena));
  if (!arena) {
    return NULL;
  }
  arena->buffer = (char *)malloc(capacity);
  if (!arena->buffer) {
    return NULL;
  }
  arena->capacity = capacity;
  arena->cursor = 0;
  return arena;
}

char *imp_arena_strdup(ImpArena *arena, const char *str) {
  size_t len = strlen(str);
  char *dup = imp_arena_alloc(arena, len + 1);
  strcpy(dup, str);
  return dup;
}

void *imp_arena_alloc(ImpArena *arena, size_t size) {
  if (!arena) {
    return NULL;
  }

  size_t aligned_current_pos = align_up(arena->cursor, IMP_ARENA_ALIGNMENT);
  size_t effective_size = size;
  size_t new_pos = aligned_current_pos + effective_size;
  if (new_pos > arena->capacity) {
    size_t new_cap = arena->capacity * 2;
    arena->buffer = (char *)realloc(arena->buffer, new_cap);
    if (!arena->buffer) {
      return NULL;
    }
    arena->capacity = new_cap;
  }
  void *ptr = (void *)(arena->buffer + aligned_current_pos);
  arena->cursor = new_pos;
  return ptr;
}

void imp_arena_reset(ImpArena *arena) {
  if (arena) {
    arena->cursor = 0;
  }
}

void imp_arena_free(ImpArena *arena) {
  if (arena) {
    if (arena->buffer) {
      free(arena->buffer);
    }
    free(arena);
  }
}
