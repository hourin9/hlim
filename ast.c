#include "hlim.h"

#include <stdlib.h>
#include <string.h>

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

struct AST *binary(enum ArtType art, struct AST *lhs, struct AST *rhs)
{
        struct AST *node = default_node();
        node->type = AST_Arit;
        node->arit = art;
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

struct AST *block(struct AST *body)
{
        struct AST *node = default_node();
        node->type = AST_Block;
        node->body = body;
        return node;
}

struct AST *branch(struct AST *cond, struct AST *then,
        struct AST *otherwise)
{
        struct AST *node = default_node();
        node->type = AST_Branch;
        node->cond = cond;
        node->args = then;
        node->args->next = otherwise;
        return node;
}

struct AST *loop(struct AST *cond, struct AST *body)
{
        struct AST *node = default_node();
        node->type = AST_Loop;
        node->cond = cond;
        node->args = body;
        return node;
}

struct AST *call(struct AST *func, struct AST *args)
{
        struct AST *call = node(AST_Call, args);
        call->func = func;
        return call;
}

struct AST *shallow_dup(const struct AST *t)
{
        struct AST *n = malloc(sizeof(*n));
        memcpy(n, t, sizeof(*n));
        return n;
}

struct AST *deep_dup(const struct AST *t)
{
        if (t == nullptr)
                return nullptr;

        struct AST *n = malloc(sizeof(*n));
        memcpy(n, t, sizeof(*n));

        if (t->sval != nullptr)
                n->sval = strdup(t->sval);

        n->lhs = deep_dup(n->lhs);
        n->rhs = deep_dup(n->rhs);

        n->args = deep_dup(n->args);
        n->params = deep_dup(n->params);
        n->next = deep_dup(n->next);

        return n;
}

