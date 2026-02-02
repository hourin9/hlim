#include "hlim.h"

#include "external/stb_ds.h"

struct InterpValue evaluate_one(RST_t *st, const struct AST *n)
{
        if (n == nullptr)
                return NIL_VALUE;

        switch (n->type) {
        case AST_Call:
                return handle_call(st, n);

        case AST_Import:
                return handle_import(st, n);

        case AST_Arit:
                if (n->arit == ART_Index)
                        return handle_indexing(st, n);
                return handle_arithmetic(st, n);

        case AST_Asn:
                return handle_asn(st, n);

        case AST_Branch:
                return handle_branching(st, n);

        case AST_Decl:
                return handle_decl(st, n);

        case AST_Id:
                return rst_find(st, n->sval);

        case AST_Loop:
                return handle_loop(st, n);

        case AST_NumericLiteral:
                return (struct InterpValue){
                        .type = VAL_Num,
                        .f32 = n->f32,
                };

        case AST_Block:
                // So that node doesn't have to be un-const'ed
                struct AST *block = deep_dup(n);
                return (struct InterpValue){
                        .type = VAL_Node,
                        .node = block,
                        .scope = st->current,
                };

        case AST_StringLiteral:
                return (struct InterpValue){
                        .type = VAL_String,
                        .str = n->sval,
                };

        default:
                return NIL_VALUE;
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

        case VAL_FFILibHandle:
                printf("handler(%p)", v.ptr);
                break;

        case VAL_FFISym:
                printf("sym(%p)", v.ptr);
                break;
        }

        fflush(stdout);
}

bool to_bool(struct InterpValue v)
{
        switch (v.type) {
        case VAL_Num:
                return v.f32 != 0;

        case VAL_FFILibHandle:
                __attribute__((fallthrough));
        case VAL_FFISym:
                return v.ptr != nullptr;

        default:
                return false;
        }
}

float to_num(struct InterpValue v)
{
        if (v.type == VAL_Num)
                return v.f32;
        return -1;
}

const char *to_str(struct InterpValue v)
{
        if (v.type == VAL_String)
                return v.str;
        return nullptr;
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

struct InterpValue *evaluate_arg_list(RST_t *rst, const struct AST *args)
{
        struct InterpValue *argv = nullptr;

        const struct AST *arg = args;
        while (arg != nullptr) {
                struct InterpValue val = evaluate_one(rst, arg);
                arrput(argv, val);
                arg = arg->next;
        }

        return argv;
}

