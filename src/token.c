#include "token.h"

Token make_token   (TokenKind kind, const char* lexme, Span span)
{
    return (Token) {kind, lexme, span};
}

Span  make_span    (int line, int column, int colend)
{
    return (Span)  {line, column, colend};
}

Span  merge_span   (Span s1, Span s2)
{
    return make_span(s1.line, s1.column, s2.colend);
}

