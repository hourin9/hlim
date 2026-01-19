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

        // FIXME: evaluate args in outer scope first

        // Anonymous function (calling a block)
        if (n->func->sval == nullptr) {
                // TODO: use closure here too
                rst_new_scope(st);
                final = evaluate_list(st, n->body->body);
                rst_pop_scope(st);
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

        struct InterpValue func = rst_find(st, n->func->sval);

        if (func.type != VAL_Node)
                return final;

        rst_closure(st, func.scope);
        final = evaluate_list(st, func.node);
        rst_pop_scope(st);
        return final;
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
                        .scope = st->current,
                };
        } else {
                v = evaluate_one(st, val);
        }

        rst_declare(st, id->sval, v);
        return v;
}

struct InterpValue handle_asn(RST_t *st, const struct AST *n)
{
        struct InterpValue v;
        struct AST *id = n->args;
        struct AST *val = n->args->next;

        if (val->type == AST_Block) {
                v = (struct InterpValue){
                        .type = VAL_Node,
                        .node = val->body,
                        .scope = st->current,
                };
        } else {
                v = evaluate_one(st, val);
        }

        rst_assign(st, id->sval, v);
        return v;
}

struct InterpValue handle_loop(RST_t *st, const struct AST *n)
{
        struct AST *body = n->args;

        while ((to_bool(evaluate_one(st, n->cond))) == true)
                evaluate_one(st, body);

        return (struct InterpValue){ .type = VAL_Nil };
}

struct InterpValue handle_arithmetic(RST_t *st, const struct AST *n)
{
        struct InterpValue val = { .type = VAL_Num };
        float rhs = to_num(evaluate_one(st, n->rhs)),
              lhs = to_num(evaluate_one(st, n->lhs));

        switch (n->arit) {
        case ART_Add:
                val.f32 = lhs + rhs;
                break;

        case ART_Sub:
                val.f32 = lhs - rhs;
                break;

        case ART_Mul:
                val.f32 = lhs * rhs;
                break;

        case ART_Div:
                val.f32 = lhs / rhs;
                break;
        }

        return val;
}

