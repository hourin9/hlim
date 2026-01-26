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

        struct InterpValue vlhs = constant_fold(node->lhs),
                           vrhs = constant_fold(node->rhs);
        if (vlhs.type == VAL_Nil && vrhs.type == VAL_Nil)
                return (struct InterpValue){ .type = VAL_Nil };

        struct AST *lhs = number(vlhs.f32),
                   *rhs = number(vrhs.f32);

        struct AST *tmp = binary(node->arit, lhs, rhs);
        struct InterpValue v = handle_arithmetic(nullptr, tmp);
        free(tmp);
        return v;
}

