#include "hlim.h"

#include "external/stb_ds.h"

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
        struct InterpValue *args,
        struct InterpValue call_handle)
{
        size_t argc = arrlen(args);
        ffi_cif cif;

        ffi_type *ffi_args[argc];
        void *values[argc];

        for (size_t i=0; i<argc; i++) {
                struct InterpValue *arg = &args[i];

                switch (arg->type) {
                case VAL_Num:
                        ffi_args[i] = &ffi_type_float;
                        values[i] = &arg->f32;
                        break;

                case VAL_Nil:
                        ffi_args[i] = &ffi_type_pointer;
                        values[i] = nullptr;
                        break;

                case VAL_String:
                        ffi_args[i] = &ffi_type_pointer;
                        values[i] = &arg->str;

                default:
                        ffi_args[i] = &ffi_type_pointer;
                        values[i] = &arg->ptr;
                        break;
                }
        }

        ffi_status status = ffi_prep_cif(
                &cif,
                FFI_DEFAULT_ABI,
                argc,
                &ffi_type_float,
                ffi_args
        );

        if (status == FFI_OK) {
                float rc;
                ffi_call(&cif, FFI_FN(call_handle.ptr), &rc, values);

                return (struct InterpValue){
                        .type = VAL_Num,
                        .f32 = rc
                };
        }

        return (struct InterpValue){ .type = VAL_Nil };
}

