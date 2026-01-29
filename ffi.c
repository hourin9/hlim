#include "hlim.h"

#include <dlfcn.h>

struct InterpValue handle_ffi_load(
        struct InterpValue arg,
        void *handle)
{
        if (arg.type != VAL_String)
                return (struct InterpValue){ .type = VAL_Nil };

        void *ptr = dlsym(handle, arg.str);

        if (!ptr)
                return (struct InterpValue){ .type = VAL_Nil };

        return (struct InterpValue){
                .type = VAL_FFISym,
                .ptr = ptr
        };
}

