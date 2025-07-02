#include "options.h"
#include <stdio.h>
#include <string.h>

#include "imp_alloc.h"
#include "global.h"

#define sflag(arg, flag) (strcmp(arg, flag) == 0)
#define lflag(arg, short, long) ((strcmp(arg, short) == 0) || (strcmp(arg, long) == 0))

char* shift(int* c, char** v)
{
    return v[(*c)++];
}

Error* init_options(Options* opt, int argc, char** argv)
{
    int i = 0;
    opt->program_path = shift(&i, argv);
    if    (argc == 1)
    {
	return make_error(ERROR_FATAL, "No Arguments were given", 1);
    }
    while (i < argc)
    {
	char* arg = shift(&i, argv);
	if (lflag(arg, "-i", "--input"))
	{
	    opt->run_option = RUN_FILE;
	    if (i >= argc)
	    {		
		return make_error(ERROR_FATAL, "Expected Input File as argument", 1);
	    }
	    opt->infile = shift(&i, argv);
	}
	else if (lflag(arg, "-h", "--help"))
	{
	    // TODO: add specialized help i.e: eml -h 'sys.exit'
	    opt->run_option = RUN_SHOW_HELP;
	    return NULL;
	}
	else
	{
	    char msg[64] = {0};
	    sprintf(msg, "Unknown Argument: '%s'", arg);
	    return make_error(ERROR_FATAL, imp_arena_strdup(arena, msg), 1);
	}
	i++;
    }
    return NULL;
}
