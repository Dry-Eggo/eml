#include "containers.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include "../global.h"

using namespace std;

struct TokenList
{
    vector<Token> tokens;
};

TokenList* make_tokenlist()
{
    TokenList* tl = (TokenList*)imp_arena_alloc(arena, sizeof(TokenList));
    return tl;
}

void tokenlist_add(TokenList* t, Token tok)
{
    t->tokens.push_back(tok);
}

Token tokenlist_get(TokenList* t, int n)
{
    return t->tokens.at(n);
}

int tokenlist_size(TokenList* t)
{
    return t->tokens.size();
}


char * readAllFile(const char* path)
{
    fstream file = fstream(path);
    stringstream s;
    s << file.rdbuf();
    string content = s.str();
    return imp_arena_strdup(arena, content.data());
}

struct AstList
{
    vector<struct AstNode*> nodes;
};

AstList* make_astlist()
{
    AstList* tl = (AstList*)imp_arena_alloc(arena, sizeof(AstList));
    return tl;
}

void astlist_add(AstList* t, struct AstNode* node)
{
    t->nodes.push_back(node);
}

struct AstNode* astlist_get(AstList* t, int n)
{
    return t->nodes.at(n);
}

int astlist_size(AstList* t)
{
    return t->nodes.size();
}


// Runtime

struct Env
{
    Env* parent;
    unordered_map<string, Value_*> vars;
};

Env* create_env(Env* parent)
{
    Env* env = (Env*)imp_arena_alloc(arena, sizeof(Env));
    if (!env)
    {
	return NULL;
    }
    env->parent = parent;
    return env;
}

struct Module
{
    const char* name;
    unordered_map<string, Value_*> exports;
};

struct ModuleTable
{
    unordered_map<string, Module*> modules;
};
