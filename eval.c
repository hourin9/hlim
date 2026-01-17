#include "hlim.h"

struct InterpValue evaluate_one(
        RST_t *st,
        const struct AST *n)
{
        if (n == nullptr)
                return (struct InterpValue){ .type = VAL_Nil };

        switch (n->type) {
        case AST_Call:
                printf("hehehe\n");
                break;

        case AST_Decl:
                printf("decl\n");
                break;

        default:
                break;
        }

        return (struct InterpValue){ 0 };
}

