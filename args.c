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

void push_args_params(RST_t *st, const struct AST *params,
        struct InterpValue *argv)
{
        if (argv == nullptr)
                return;

        const struct AST *param = params;
        size_t unnamed = 0;
        for (size_t i=0; i<arrlen(argv); i++) {
                if (param) {
                        rst_declare(st, param->sval, argv[i]);
                        param = param->next;
                } else {
                        char name[512];
                        snprintf(name, sizeof(name), "r%zu", unnamed);
                        rst_declare(st, strdup(name), argv[i]);
                        unnamed ++;
                }
        }
}

