#include "hlim.h"

#include <string.h>

struct AST *to_ast(struct InterpValue v)
{
        switch (v.type) {
        case VAL_String:
                return string(strdup(v.str));

        case VAL_Num:
                return number(v.f32);

        case VAL_Node:
                return v.node;

        default:
                return default_node();
        }
}

