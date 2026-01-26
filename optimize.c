#include "hlim.h"

struct AST *optimize(struct AST *n)
{
        if (n == nullptr)
                return nullptr;

        if (n->type == AST_Arit || n->type == AST_Asn) {
                n->lhs = optimize(n->lhs);
                n->rhs = optimize(n->rhs);
        }

        if (n->type == AST_Arit) {
                struct InterpValue res = constant_fold(n);
                if (res.type == VAL_Num)
                        return number(res.f32);
        }

        if (n->next)
                n->next = optimize(n->next);

        return n;
}

