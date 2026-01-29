#include "hlim.h"

#include <dlfcn.h>

struct InterpValue handle_import(RST_t *t, const struct AST *n)
{
        const char *lib = to_str(evaluate_one(t, n->args));

        if (lib != nullptr) {
                void *handle = dlopen(lib, RTLD_LAZY);

                if (handle)
                        return (struct InterpValue){
                                .type = VAL_FFILibHandle,
                                .ptr = handle
                        };
        }

        return (struct InterpValue){ .type = VAL_Nil };
}

