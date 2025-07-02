#include "containers.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

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
