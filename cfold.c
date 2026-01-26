#include "hlim.h"

#include <stdlib.h>

struct InterpValue constant_fold(const struct AST *node)
{
        if (node == nullptr)
                return (struct InterpValue){ .type = VAL_Nil };

        if (node->type == AST_NumericLiteral)
                return (struct InterpValue){
                        .type = VAL_Num,
                        .f32 = node->f32
                };

        if (node->type != AST_Arit)
                return (struct InterpValue){ .type = VAL_Nil };

        struct InterpValue lhs = constant_fold(node->lhs),
                           rhs = constant_fold(node->rhs);
        if (lhs.type == VAL_Nil || rhs.type == VAL_Nil)
                return (struct InterpValue){ .type = VAL_Nil };

        struct AST *tmp = binary(
                node->arit,
                number(lhs.type),
                number(lhs.type)
        );
        struct InterpValue v = handle_arithmetic(nullptr, tmp);
        free(tmp);
        return v;
}

