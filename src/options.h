#pragma once

#include <stdbool.h>

#include "enums.h"
#include "error.h"

typedef struct
{
    RunOption   run_option;
    const char* infile;
    const char* program_path;
} Options;

Error* init_options(Options * opt, int argc, char** argv);

