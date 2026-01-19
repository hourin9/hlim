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
        struct SSTWrapper *scope = malloc(sizeof(*scope));
        struct InterpValue nil = { .type = VAL_Nil };
        scope->table = nullptr;
        scope->parent = rst->current;
        shdefault(scope->table, nil);
        rst->current = scope;
}

void rst_pop_scope(RST_t *rst)
{
        if (rst->current->parent == nullptr)
                return;
        struct SSTWrapper *old = rst->current;
        rst->current = old->parent;
}

void rst_closure(RST_t *rst, struct SSTWrapper *capture)
{
        struct SSTWrapper *scope = malloc(sizeof(*scope));
        scope->table = nullptr;

        scope->parent = capture;
        rst->current = scope;
}

SST_t *global_rt_scope(RST_t *rst)
{
        struct SSTWrapper *cur = rst->current;
        while (cur->parent != nullptr)
                cur = cur->parent;
        return cur->table;
}

SST_t *current_rt_scope(RST_t *rst)
{
        return rst->current->table;
}

void rst_declare(RST_t *rst, char *id, struct InterpValue val)
{
        shput(rst->current->table, id, val);
}

void rst_assign(RST_t *rst, char *id, struct InterpValue val)
{
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

struct InterpValue rst_find(RST_t *rst, char *id)
{
        struct SSTWrapper *cur = rst->current;
        while (cur != nullptr) {
                int index = shgeti(cur->table, id);
                if (index != -1)
                        return cur->table[index].value;
                cur = cur->parent;
        }

        return (struct InterpValue){ .type = VAL_Nil };
}

