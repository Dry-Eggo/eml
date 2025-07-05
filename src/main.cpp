#include <stdio.h>
#include "options.h"
#include "imp_alloc.h"
#include "interpreter.hpp"

ImpArena *arena = NULL;

extern void show_help(Options opt)
{
    fprintf(stdout, "Usage: %s [option] [args..]\n", opt.program_path);
    fprintf(stdout, "option:\n");
    fprintf(stdout, "    -i, --input            specify input file or <stdin> for repl\n");
    fprintf(stdout, "    -h, --help             show this help message\n");
}

int main(int argc, char** argv)
{
    arena = imp_arena_init(1024);
    
    Options opt;
    Error* e    = init_options(&opt, argc, argv);
    if (e)
    {
	early_fail(e);
    }

    switch(opt.run_option)
    {
	case RUN_SHOW_HELP:
	    show_help(opt);
	    break;
	case RUN_FILE:
	    interpret(&opt);
	    break;
	default:
	    break;
    }
    imp_arena_free(arena);
}
