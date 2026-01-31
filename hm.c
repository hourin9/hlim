#include "hlim.h"

struct Type *hm_find(struct Type *t)
{
        // t is fully resolved.
        if (t->parent == nullptr)
                return t;
        return t->parent = hm_find(t->parent);
}

bool hm_unify(struct Type *a, struct Type *b)
{
        a = hm_find(a);
        b = hm_find(b);

        if (a == b)
                return true;

        // TODO: stuff

        return false;
}

