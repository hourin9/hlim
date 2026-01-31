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

        // 1st pass: type checking
        struct AST *check_cur = parser_ast;
        while (check_cur != nullptr) {
                check_cur = check_cur->next;
        }

        // 2nd pass: evaluation
        struct AST *eval_cur = parser_ast;
        while (eval_cur != nullptr) {
                evaluate_one(&rst, eval_cur);
                eval_cur = eval_cur->next;
        }

        return 0;
}

