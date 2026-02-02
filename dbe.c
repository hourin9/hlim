#include "hlim.h"

struct InterpValue eliminate_dead_branches(struct AST *n)
{
        if (n->type != AST_Branch && n->type != AST_Loop)
                return NIL_VALUE;

        struct InterpValue cond = constant_fold(n->cond);
        if (cond.type == VAL_Nil)
                return NIL_VALUE;

        if (n->type == AST_Branch) {
                struct AST *result = nullptr;

                // Always true
                if (to_bool(cond)) {
                        result = dup(n->args);

                        // Clear the else branch
                        result->next = nullptr;
                }

                // Always false
                else {
                        result = dup(n->args->next);
                }

                return (struct InterpValue){
                        .type = VAL_Node,
                        .node = result
                };
        }

        else {
                if (!to_bool(cond)) {
                        struct AST *next = n->next;
                        return (struct InterpValue){
                                .type = VAL_Node,
                                .node = (next != nullptr)
                                        ? dup(next)
                                        : nullptr,
                        };
                }

                return NIL_VALUE;
        }
}

