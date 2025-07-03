#pragma once

#include "options.h"
#include "misc/containers.h"
#include "global.h"

typedef struct
{
    Env* global_env;
    Env* current_env;
} Runtime;


void interpret(Options* opt);
Error* init_runtime();
Error* init_env();
