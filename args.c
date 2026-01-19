#include "hlim.h"

#include <stdio.h>

#include "external/stb_ds.h"

void push_args_simple(RST_t *st, struct InterpValue *argv)
{
        if (argv == nullptr)
                return;

        for (size_t i=0; i<arrlen(argv); i++) {
                char name[512];
                snprintf(name, sizeof(name), "r%zu", i);
                rst_declare(st, strdup(name), argv[i]);
        }
}

