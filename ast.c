#include "hlim.h"

#include <stdlib.h>

struct AST *node(enum ASTType t, struct AST *lhs, struct AST *rhs)
{
        struct AST *node = malloc(sizeof(*node));
        *node = (struct AST){ 0 };
        node->type = t;
        node->lhs = lhs;
        node->rhs = rhs;
        return node;
}

struct AST *leaf(enum ASTType t, char *sval)
{
        struct AST *node = malloc(sizeof(*node));
        *node = (struct AST){ 0 };
        node->type = t;
        node->sval = sval;
        return node;
}

struct AST *number(float f32)
{
        struct AST *node = malloc(sizeof(*node));
        *node = (struct AST){ 0 };
        node->type = AST_NumericLiteral;
        node->f32 = f32;
        return node;
}

