#include "hlim.h"

#include <stdio.h>

void recursive_print(FILE *where, const struct AST *node,
        int depth, bool follow)
{
        if (!node)
                return;

        fprintf(where, "%*s[%d]", depth, "", node->type);
        if (node->sval)
                fprintf(where, "sval: %s", node->sval);
        if (node->type == AST_NumericLiteral)
                fprintf(where, "f32: %g", node->f32);
        fprintf(where, "\n");

        if (node->lhs) {
                fprintf(where, "%*sLHS:\n", depth, "");
                recursive_print(where, node->lhs, depth + 4, follow);
        }


        if (node->rhs) {
                fprintf(where, "%*sRHS:\n", depth, "");
                recursive_print(where, node->rhs,
                        depth + 4, (node->type == AST_Block));
        }

        if (node->args) {
                fprintf(where, "%*sARGS: (\n", depth, "");
                recursive_print(where, node->args, depth + 4, follow);
                fprintf(where, "%*s)\n", depth, "");
        }

        if (!follow)
                return;

        if (node->next)
                recursive_print(where, node->next, depth, follow);
}

