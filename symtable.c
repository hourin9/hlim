#include "hlim.h"

#include "external/stb_ds.h"

struct RuntimeSymTable init_runtime_symtable()
{
        struct RuntimeSymTable rst = { 0 };
        return rst;
}

void rst_new_scope(struct RuntimeSymTable *rst)
{
        arrput(rst->levels, nullptr);
        rst->current ++;
}

