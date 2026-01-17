#include <stdio.h>

#include "gram.tab.h"
#include "hlim.h"

#define STB_DS_IMPLEMENTATION
#include "external/stb_ds.h"

extern struct AST *parser_ast;

int main(int argc, char **argv)
{
        if (yyparse() != 0)
                return 0;

        recursive_print(stderr, parser_ast, 0);
        return 0;
}

