#include "hlim.h"

struct InterpValue evaluate_one(RST_t *st, const struct AST *n)
{
        if (n == nullptr)
                return (struct InterpValue){ .type = VAL_Nil };

        switch (n->type) {
        case AST_Call:
                return handle_call(st, n);

        case AST_Branch:
                return handle_branching(st, n);

        case AST_Decl:
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
                return (struct InterpValue){ .type = VAL_Nil };

        case AST_Id:
                return rst_find_one_scope(st, n->sval, st->current);

        case AST_NumericLiteral:
                return (struct InterpValue){
                        .type = VAL_Num,
                        .f32 = n->f32,
                };

        case AST_StringLiteral:
                return (struct InterpValue){
                        .type = VAL_String,
                        .str = n->sval,
                };

        default:
                return (struct InterpValue){ .type = VAL_Nil };
        }
}

void print_value(struct InterpValue v)
{
        switch (v.type) {
        case VAL_Nil:
                printf("(NIL)");
                break;

        case VAL_Id:
                printf("(%s)", v.str);
                break;

        case VAL_Num:
                printf("%g", v.f32);
                break;

        case VAL_String:
                printf("%s", v.str);
                break;

        case VAL_Node:
                recursive_print(stdout, v.node, 0);
                break;
        }

        fflush(stdout);
}

bool to_bool(struct InterpValue v)
{
        switch (v.type) {
        case VAL_Num:
                return v.f32 != 0;

        default:
                return false;
        }
}

struct InterpValue evaluate_block(RST_t *rst, const struct AST *root)
{
        if (root->type == AST_Block)
                return evaluate_list(rst, root->body);
        return evaluate_one(rst, root);
}

struct InterpValue evaluate_list(RST_t *rst, const struct AST *root)
{
        const struct AST *cur = root;
        struct InterpValue v = { 0 };
        while (cur != nullptr) {
                v = evaluate_one(rst, cur);
                cur = cur->next;
        }
        return v;
}

