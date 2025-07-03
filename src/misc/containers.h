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

struct AstNode;
typedef struct AstList AstList;
AstList*             make_astlist();
void                 astlist_add(struct AstList*, struct AstNode* node);
struct AstNode*      astlist_get(struct AstList*, int i);
int                  astlist_size(struct AstList*);


/* ------------------ Runtime ----------------- */

typedef struct Env Env;
typedef struct Vm  Vm;
typedef struct Module Module;
typedef struct ModuleTable ModuleTable;
struct Value_;

Env*          create_env(Env* parent);
struct Value_ get_var(Env* e, const char* name);
struct Value_ set_var(Env* e, const char* name, struct Value_ value);
Module*       get_module(Env* e, const char* name);
Module*       load_module_from_file(Env* e, const char* path);
Module*       load_system_module(Env* e, const char* name);

/* -------------------------------------------- */


/* ------------------ utils ------------------ */
char*      readAllFile(const char* path);
/* ------------------------------------------- */

#ifdef __cplusplus
}
#endif
