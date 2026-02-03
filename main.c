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

        RST_t rst = init_runtime_symtable();
        struct AST *cur = parser_ast;
        while (cur != nullptr) {
                evaluate_one(&rst, cur);
                cur = cur->next;
        }

        deep_del(parser_ast);
        destroy_runtime_symtable(&rst);
        return 0;
}

