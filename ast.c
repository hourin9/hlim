#include "hlim.h"

#include <stdlib.h>

struct AST *find_end(struct AST *node)
{
        struct AST *end = node;
        while (end->next)
                end = end->next;
        return end;
}

struct AST *append_arg(struct AST *arg, struct AST *argv)
{
        if (argv == nullptr)
                return arg;

        argv = find_end(argv);
        argv->next = arg;
        return arg;
}

struct AST *default_node()
{
        struct AST *node = malloc(sizeof(*node));
        *node = (struct AST){ 0 };
        return node;
}

struct AST *node(enum ASTType t, struct AST *args)
{
        struct AST *node = default_node();
        node->type = t;
        node->args = args;
        return node;
}

struct AST *binary(struct AST *lhs, struct AST *rhs)
{
        struct AST *node = default_node();
        node->type = AST_NumericLiteral;
        node->lhs = lhs;
        node->rhs = rhs;
        return node;
}

struct AST *leaf(enum ASTType t, char *sval)
{
        struct AST *node = default_node();
        node->type = t;
        node->sval = sval;
        return node;
}

struct AST *number(float f32)
{
        struct AST *node = default_node();
        node->type = AST_NumericLiteral;
        node->f32 = f32;
        return node;
}

struct AST *string(char *str)
{
        struct AST *node = default_node();
        node->type = AST_StringLiteral;
        node->sval = str;
        return node;
}

struct AST *id(char *sval)
{
        struct AST *node = default_node();
        node->type = AST_Id;
        node->id = sval;
        return node;
}

