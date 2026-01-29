#include "hlim.h"

#include <dlfcn.h>

struct InterpValue handle_import(RST_t *t, const struct AST *n)
{
        int imported_syms = -1;

        const char *lib = to_str(evaluate_one(t, n->args));

        if (lib != nullptr) {
                imported_syms = 0;
                void *handle = dlopen(lib, RTLD_LAZY | RTLD_GLOBAL);

                if (!handle)
                        imported_syms = -1;
                else {
                        // TODO: handle ffi sym loading
                }
        }

        return (struct InterpValue){
                .type = VAL_Num,
                .f32 = imported_syms
        };
}

