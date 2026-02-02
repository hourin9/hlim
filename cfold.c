#include "hlim.h"

#include <stdlib.h>

struct InterpValue constant_fold(const struct AST *node)
{
        if (node == nullptr)
                return NIL_VALUE;

        if (node->type == AST_NumericLiteral)
                return (struct InterpValue){
                        .type = VAL_Num,
                        .f32 = node->f32
                };

        if (node->type != AST_Arit)
                return NIL_VALUE;

        struct InterpValue lhs = constant_fold(node->lhs),
                           rhs = constant_fold(node->rhs);
        if (lhs.type == VAL_Nil || rhs.type == VAL_Nil)
                return NIL_VALUE;

        struct AST *tmp = binary(
                node->arit,
                number(lhs.f32),
                number(rhs.f32)
        );
        struct InterpValue v = handle_arithmetic(nullptr, tmp);
        free(tmp);
        return v;
}

