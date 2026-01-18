#include "hlim.h"

#include "external/stb_ds.h"

RST_t init_runtime_symtable()
{
        RST_t rst = { 0 };
        rst_new_scope(&rst);
        return rst;
}

void rst_new_scope(RST_t *rst)
{
        rst->current ++;
        arrput(rst->levels, nullptr);

        struct InterpValue nil = { .type = VAL_Nil };
        shdefault(rst->levels[rst->current], nil);
}

SST_t *global_rt_scope(RST_t *rst)
{
        return rst->levels[0];
}

SST_t *current_rt_scope(RST_t *rst)
{
        return rst->levels[rst->current];
}

void rst_set(RST_t *rst, char *id, struct InterpValue val)
{
        shput(rst->levels[rst->current], id, val);
}

struct InterpValue rst_find_one_scope(RST_t *rst, char *id, size_t scope)
{
        if (scope > rst->current)
                return (struct InterpValue){ .type = VAL_Nil };

        return shget(rst->levels[rst->current], id);
}

