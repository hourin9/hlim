#include "hlim.h"

#include <stdio.h>
#include <unistd.h>

extern void yyrestart(FILE*);
extern int yyparse();
extern struct AST *parser_ast;

struct InterpValue handle_include(RST_t *st, struct InterpValue arg)
{
        if (arg.type != VAL_String)
                return NIL_VALUE;

        FILE *f = fopen(arg.str, "r");
        if (!f)
                return NUM_VALUE(-1);

        yyrestart(f);

        if (yyparse() != 0)
                return NIL_VALUE;

        struct AST *cur = parser_ast;
        while (cur != nullptr) {
                evaluate_one(st, cur);
                cur = cur->next;
        }

        fclose(f);

        return NUM_VALUE(0);
}

