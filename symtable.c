#include "hlim.h"

#include <stdlib.h>

#include "external/stb_ds.h"

RST_t init_runtime_symtable()
{
        RST_t rst = { 0 };
        struct SSTWrapper *global = malloc(sizeof(*global));
        global->table = nullptr;
        global->parent = nullptr;
        rst.current = global;
        return rst;
}

void rst_new_scope(RST_t *rst)
{
        if (rst == nullptr)
                return;
        struct SSTWrapper *scope = malloc(sizeof(*scope));
        struct InterpValue nil = { .type = VAL_Nil };
        scope->table = nullptr;
        scope->parent = rst->current;
        shdefault(scope->table, nil);
        rst->current = scope;
}

void rst_pop_scope(RST_t *rst)
{
        if (rst == nullptr)
                return;
        if (rst->current->parent == nullptr)
                return;
        struct SSTWrapper *old = rst->current;
        rst->current = old->parent;
}

void rst_closure(RST_t *rst, struct SSTWrapper *capture)
{
        if (rst == nullptr)
                return;
        struct SSTWrapper *scope = malloc(sizeof(*scope));
        scope->table = nullptr;

        scope->parent = capture;
        rst->current = scope;
}

SST_t *global_rt_scope(RST_t *rst)
{
        if (rst == nullptr)
                return nullptr;
        struct SSTWrapper *cur = rst->current;
        while (cur->parent != nullptr)
                cur = cur->parent;
        return cur->table;
}

SST_t *current_rt_scope(RST_t *rst)
{
        if (rst == nullptr)
                return nullptr;
        return rst->current->table;
}

void rst_declare(RST_t *rst, char *id, struct InterpValue val)
{
        if (rst == nullptr)
                return;
        shput(rst->current->table, id, val);
}

void rst_assign(RST_t *rst, char *id, struct InterpValue val)
{
        if (rst == nullptr)
                return;
        struct SSTWrapper *cur = rst->current;

        while (cur != NULL) {
                int index = shgeti(cur->table, id);

                if (index != -1) {
                        cur->table[index].value = val;
                        return;
                }

                cur = cur->parent;
        }
}

void rst_assign_index(RST_t *rst, char *id, int idx,
        struct InterpValue val)
{
        if (rst == nullptr)
                return;
        struct SSTWrapper *cur = rst->current;

        while (cur != NULL) {
                int index = shgeti(cur->table, id);

                if (index != -1) {
                        struct InterpValue *list =
                                &cur->table[index].value;

                        if (list->type != VAL_Node)
                                return;

                        struct AST *cur = list->node->body;

                        // TODO: append to list if idx is -1.

                        for (size_t i=0; i<(size_t)idx; i++)
                                cur = cur->next;

                        struct AST *next = cur->next;
                        cur = to_ast(val);
                        cur->next = next;

                        return;
                }

                cur = cur->parent;
        }
}

struct InterpValue rst_find(RST_t *rst, char *id)
{
        if (rst == nullptr)
                return (struct InterpValue){ .type = VAL_Nil };
        struct SSTWrapper *cur = rst->current;
        while (cur != nullptr) {
                int index = shgeti(cur->table, id);
                if (index != -1)
                        return cur->table[index].value;
                cur = cur->parent;
        }

        return (struct InterpValue){ .type = VAL_Nil };
}

