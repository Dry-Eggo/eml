#include "error.h"
#include "global.h"

Error* make_error     (ErrorKind kind, const char* message, int code)
{
    Error* e   = imp_arena_alloc(arena, sizeof(Error));
    e->message = message;
    e->code    = code;
    e->kind    = kind;
    return e;
}

void   early_fail     (Error* e)
{
    fprintf(stderr, "EML: %s\n", e->message);
    fprintf(stderr, "exited with code %d\n", e->code);
    exit(e->code);
}
