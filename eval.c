#include "hlim.h"

#include <string.h>

struct InterpValue evaluate_one(
        RST_t *st,
        const struct AST *n)
{
        if (n == nullptr)
                return (struct InterpValue){ .type = VAL_Nil };

        switch (n->type) {
        case AST_Call:
                // TODO: move this to separate builtin func handler
                if (strcmp(n->func->sval, "print") == 0) {
                        const struct AST *arg = n->args;
                        while (arg != nullptr) {
                                struct InterpValue val = evaluate_one(st, arg);
                                print_value(val);
                                arg = arg->next;
                        }
                }
                return (struct InterpValue){ .type = VAL_Nil };

        case AST_Decl:
                printf("decl\n");
                return (struct InterpValue){ .type = VAL_Nil };

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
}

