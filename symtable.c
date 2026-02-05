#include "hlim.h"

#include <stdlib.h>

#include "external/stb_ds.h"

struct SSTWrapper *new_closure(struct SSTWrapper *par)
{
        struct SSTWrapper *sw = malloc(sizeof(*sw));
        sw->table = nullptr;
        sw->parent = par;
        sw->ref_count = 1;
        if (par)
                sw->parent->ref_count ++;
        return sw;
}

void clean_closure(struct SSTWrapper *cl)
{
        if (cl == nullptr)
                return;

        for (size_t i=0; i<shlen(cl->table); i++) {
                struct InterpValue *v = &cl->table[i].value;

                if (v->type == VAL_String)
                        free(v->str);

                if (v->type != VAL_Node)
                        continue;

                // TODO: implement ref counting for AST nodes.
        }
}

void decref_closure(struct SSTWrapper *cl)
{
        if (cl == nullptr)
                return;

        cl->ref_count --;

        if (cl->ref_count <= 0) {
                struct SSTWrapper *p = cl->parent;

                clean_closure(cl);
                shfree(cl->table);
                free(cl);

                decref_closure(p);
        }
}

RST_t init_runtime_symtable()
{
        RST_t rst = { 0 };
        rst.current = new_closure(nullptr);
        return rst;
}

void destroy_runtime_symtable(RST_t *st)
{
        if (!st || !st->current)
                return;

        struct SSTWrapper *cur = st->current;
        while (cur != nullptr) {
                struct SSTWrapper *par = cur->parent;

                for (int i=0; i<shlen(cur->table); i++) {
                        // I freed the keys somewhere...
                        // free(cur->table[i].key);

                        struct InterpValue v = cur->table[i].value;
                        if (v.type == VAL_Node) {
                                if (v.node->shallow_copy) {
                                        shallow_del(v.node);
                                        free(v.node);
                                }
                                else
                                        deep_del(v.node);
                        }
                }

                shfree(cur->table);
                free(cur);

                cur = par;
        }

        st->current = nullptr;
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
        rst->current = new_closure(capture);
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

static void _rst_append_list(
        struct AST **list,
        struct InterpValue v)
{
        while (*list != nullptr)
                list = &((*list)->next);

        *list = to_ast(v);
}

void rst_assign_index(RST_t *rst, char *id, int idx,
        struct InterpValue val)
{
        if (rst == nullptr)
                return;
        struct SSTWrapper *scope = rst->current;

        while (scope != NULL) {
                int index = shgeti(scope->table, id);

                if (index == -1) {
                        scope = scope->parent;
                        continue;
                }

                struct InterpValue *list =
                        &scope->table[index].value;

                if (list->type != VAL_Node)
                        return;

                struct AST **cur = &list->node->body;

                if (idx == -1)
                        return _rst_append_list(cur, val);

                for (size_t i=0; i<(size_t)idx; i++)
                        cur = &((*cur)->next);

                struct AST *old = *cur,
                                *new = to_ast(val);

                new->next = old->next;
                *cur = new;

                // TODO: perhaps free the old node.

                return;
        }
}

struct InterpValue rst_find(RST_t *rst, char *id)
{
        if (rst == nullptr)
                return NIL_VALUE;
        struct SSTWrapper *cur = rst->current;
        while (cur != nullptr) {
                int index = shgeti(cur->table, id);
                if (index != -1)
                        return cur->table[index].value;
                cur = cur->parent;
        }

        return NIL_VALUE;
}

