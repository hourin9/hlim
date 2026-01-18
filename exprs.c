#include "hlim.h"

#include <string.h>

struct InterpValue handle_branching(RST_t *st, const struct AST *n)
{
        struct InterpValue result = { .type = VAL_Nil };
        bool would = to_bool(evaluate_one(st, n->cond));

        if (would) {
                result = evaluate_one(st, n->args);
        } else {
                struct AST *otherwise = n->args->next;
                if (otherwise != nullptr)
                        result = evaluate_one(st, otherwise);
        }

        return result;
}

struct InterpValue handle_call(RST_t *st, const struct AST *n)
{
        struct InterpValue final = { .type = VAL_Nil };

        // Anonymous function (calling a block)
        if (n->func->sval == nullptr) {
                final = evaluate_list(st, n->body->body);
                return final;
        }

        // TODO: move this to separate builtin func handler
        if (strcmp(n->func->sval, "print") == 0) {
                const struct AST *arg = n->args;
                while (arg != nullptr) {
                        struct InterpValue val =
                                evaluate_one(st, arg);
                        print_value(val);
                        arg = arg->next;
                }

                return final;
        }

        struct InterpValue func = rst_find_one_scope(
                st,
                n->func->sval,
                st->current);

        if (func.type != VAL_Node)
                return final;

        return evaluate_list(st, func.node);
}

struct InterpValue handle_decl(RST_t *st, const struct AST *n)
{
        struct InterpValue v;
        struct AST *id = n->args;
        struct AST *val = n->args->next;

        if (val->type == AST_Block) {
                v = (struct InterpValue){
                        .type = VAL_Node,
                        .node = val->body,
                };
        } else {
                v = evaluate_one(st, val);
        }

        rst_set(st, id->sval, v);
        return v;
}

struct InterpValue handle_loop(RST_t *st, const struct AST *n)
{
        struct AST *body = n->args;

        while ((to_bool(evaluate_one(st, n->cond))) == true)
                evaluate_one(st, body);

        return (struct InterpValue){ .type = VAL_Nil };
}

