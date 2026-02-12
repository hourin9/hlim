#include "hlim.h"

#include <math.h>
#include <string.h>

#include "external/stb_ds.h"

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
        struct SSTWrapper *caller_scope = st->current;
        struct InterpValue final = { .type = VAL_Nil };
        struct InterpValue *args = evaluate_arg_list(st, n->args);
        struct InterpValue func;

        // Anonymous function (calling a block)
        if (n->func->sval == nullptr) {
                func = evaluate_one(st, n->body);
        }

        // TODO: move this to separate builtin func handler
        else if (strcmp(n->func->sval, "print") == 0) {
                for (size_t i=0; i<arrlen(args); i++)
                        print_value(args[i]);

                arrfree(args);
                return final;
        }

        else if (strcmp(n->func->sval, "assert") == 0) {
                struct InterpValue res = args[0];

                // Assertion succeeded
                if (to_bool(res)) {
                        arrfree(args);
                        return final;
                }

                // Assertion failed
                for (size_t i=1; i<arrlen(args); i++)
                        print_value(args[i]);

                // TODO: use a Result struct or something to handle returns
                // and exits instead of directly calling exit() from func.
                arrfree(args);
                exit(-1);
        }

        // Search symtable
        else {
                func = rst_find(st, n->func->sval);
        }

        if (func.type == VAL_FFILibHandle) {
                final = handle_ffi_load(args, func.ptr);
                arrfree(args);
                return final;
        }

        if (func.type == VAL_FFISym) {
                final = handle_ffi_call(args, func);
                arrfree(args);
                return final;
        }

        if (func.node == nullptr || func.type != VAL_Node) {
                arrfree(args);
                return final;
        }

        rst_closure(st, func.scope);
        push_args_params(st, func.node->params, args);
        final = evaluate_list(st, func.node->body);

        struct SSTWrapper *old_scope = st->current;
        st->current = caller_scope;
        decref_closure(old_scope);

        arrfree(args);
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
                        .node = deep_dup(val),
                        .scope = st->current,
                };
        } else {
                v = evaluate_one(st, val);
        }

        rst_declare(st, id->sval, v);
        return v;
}

struct InterpValue handle_drop(RST_t *st, const struct AST *n)
{
        const struct AST *id = n->args;
        if (id == nullptr)
                return NIL_VALUE;

        if (id->type != AST_Id && id->sval == nullptr)
                return NIL_VALUE;

        fprintf(stderr, "dropping %s\n", id->sval);
        rst_drop(st, id->sval);
        return NIL_VALUE;
}

// Returns identifier for list index operation
static char *_leftmost_index(const struct AST *root)
{
        if (root->type == AST_Id)
                return root->sval;
        return _leftmost_index(root->lhs);
}

struct InterpValue handle_asn(RST_t *st, const struct AST *n)
{
        struct InterpValue v;
        struct AST *id = n->args;
        struct AST *val = n->args->next;

        if (val->type == AST_Block) {
                v = (struct InterpValue){
                        .type = VAL_Node,
                        .node = deep_dup(val),
                        .scope = st->current,
                };
        } else {
                v = evaluate_one(st, val);
        }

        if (id->type == AST_Arit && id->arit == ART_Index) {
                int index = to_num(evaluate_one(st, id->rhs));

                // TODO: make this work with multi-level nested lists
                rst_assign_index(st, _leftmost_index(id), index, v);
        }
        else
                rst_assign(st, id->sval, v);
        return v;
}

struct InterpValue handle_loop(RST_t *st, const struct AST *n)
{
        struct AST *body = n->args;

        while ((to_bool(evaluate_one(st, n->cond))) == true)
                evaluate_one(st, body);

        return NIL_VALUE;
}

struct InterpValue handle_indexing(RST_t *st, const struct AST *n)
{
        struct InterpValue list = evaluate_one(st, n->lhs);
        if (list.type != VAL_Node)
                return NIL_VALUE;

        size_t index = to_num(evaluate_one(st, n->rhs));

        struct AST *cur = list.node->body;
        for (size_t i=0; i<index; i++)
                cur = cur->next;

        return evaluate_one(st, cur);
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

        case ART_Mod:
                val.f32 = fmodf(lhs, rhs);
                break;

        case ART_Eq:
                val.f32 = (int)(lhs == rhs);
                break;

        case ART_Neq:
                val.f32 = (int)(lhs != rhs);
                break;

        case ART_Not:
                val.f32 = !lhs;
                break;

        default:
                break;
        }

        return val;
}

