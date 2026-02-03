#include "hlim.h"

#include <stdio.h>
#include <unistd.h>

typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_create_buffer(FILE *file, int size);
extern void yy_switch_to_buffer(YY_BUFFER_STATE new_buffer);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern int yyparse();
extern struct AST *parser_ast;

struct InterpValue handle_include(RST_t *st, struct InterpValue arg)
{
        if (arg.type != VAL_String)
                return NIL_VALUE;

        FILE *f = fopen(arg.str, "r");
        if (!f)
                return NUM_VALUE(-1);

        YY_BUFFER_STATE buf = yy_create_buffer(f, BUFSIZ);
        yy_switch_to_buffer(buf);

        if (yyparse() != 0)
                return NIL_VALUE;

        struct AST *cur = parser_ast;
        while (cur != nullptr) {
                evaluate_one(st, cur);
                cur = cur->next;

                // TODO: destroy the included tree's AST
        }

        yy_delete_buffer(buf);
        fclose(f);

        return NUM_VALUE(0);
}

