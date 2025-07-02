#pragma once

#include "../token.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TokenList TokenList;
TokenList* make_tokenlist();
void       tokenlist_add(TokenList*, Token tok);
Token      tokenlist_get(TokenList*, int i);
int        tokenlist_size(TokenList*);

/* ------------------ utils ------------------ */
char*      readAllFile(const char* path);
/* ------------------------------------------- */

#ifdef __cplusplus
}
#endif
