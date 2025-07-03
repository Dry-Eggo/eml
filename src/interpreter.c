

#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "types.h"

void interpret(Options* opt)
{
    Lexer* lexer = lexer_init(opt);
    Parser* parser = parser_init(opt, lexer->source, lexer->tokens);
}

Error* init_runtime()
{
    Error* e = NULL;
    runtime = imp_arena_alloc(arena, sizeof(Runtime));
    if (!runtime)
    {
	return make_error(ERROR_FATAL, "Unable to start runtime", 1);
    }
    e = init_env();
    return e;
}

Error* init_env()
{
    runtime->global_env = create_env(NULL);
    if (!runtime->global_env)
    {
	return make_error(ERROR_FATAL, "Unable to initialize global environment", 1);
    }
    runtime->current_env = runtime->global_env;
}
