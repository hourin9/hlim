#include "hlim.h"

static bool _side_effect_cont(const struct AST *root)
{
        bool result = has_side_effect(root);

        if (root == nullptr)
                return false;

        if (result)
                return result;

        return _side_effect_cont(root->next);
}

static bool _func_has_side_fx(const struct AST *n)
{
        const struct AST *func = n->func;

        if (func->type == AST_Block) {
                if (func->body == nullptr)
                        return false;
                return _side_effect_cont(func->body);
        }

        return true;
}

bool has_side_effect(const struct AST *n)
{
        if (n == nullptr)
                return false;

        if (has_side_effect(n->lhs))
                return true;

        if (has_side_effect(n->rhs))
                return true;

        switch (n->type) {
        case AST_Asn:
        case AST_Decl:
                return true;

        case AST_Call:
                return _func_has_side_fx(n);

        case AST_Block:
                return _side_effect_cont(n->body);

        case AST_Loop:
                return has_side_effect(n->args);

        case AST_Branch:
                return has_side_effect(n->args) ||
                        has_side_effect(n->args->next);

        default:
                return false;
        }
}

