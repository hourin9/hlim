#include "hlim.h"

#include "external/stb_ds.h"

RST_t init_runtime_symtable()
{
        RST_t rst = { 0 };
        return rst;
}

void rst_new_scope(RST_t *rst)
{
        arrput(rst->levels, nullptr);
        rst->current ++;
}

SST_t *global_rt_scope(RST_t *rst)
{
        return rst->levels[0];
}

SST_t *current_rt_scope(RST_t *rst)
{
        return rst->levels[rst->current];
}

