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

        if (n->type == AST_Block) {
                n->body = optimize(n->body);
        }

        struct InterpValue db_elim = eliminate_dead_branches(n);
        if (db_elim.type == VAL_Node)
                // In case it eliminates a loop, current node is
                // replaced with the next node. Thus shortening the AST
                // and making the `if (n->next)` wrong.
                // We try to optimize the node it returns in that case.
                return optimize(db_elim.node);

        if (n->next)
                n->next = optimize(n->next);

        return n;
}

