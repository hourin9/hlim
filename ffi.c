#include "hlim.h"

#include <dlfcn.h>
#include <ffi.h>

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

struct InterpValue handle_ffi_call(
        const struct InterpValue *args,
        struct InterpValue call_handle)
{
        ffi_cif cif;
        ffi_type *ffi_args[1];
        void *values[1];

        float arg_val = args->f32;
        float rc;

        ffi_args[0] = &ffi_type_float;
        values[0] = &arg_val;


        ffi_status status = ffi_prep_cif(
                &cif,
                FFI_DEFAULT_ABI,
                1,
                &ffi_type_float,
                ffi_args
        );

        if (status == FFI_OK) {
                ffi_call(&cif, FFI_FN(call_handle.ptr), &rc, values);

                return (struct InterpValue){
                        .type = VAL_Num,
                        .f32 = rc
                };
        }

        return (struct InterpValue){ .type = VAL_Nil };
}

