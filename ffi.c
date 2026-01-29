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

        // AI told me use to use a storage buffer for
        // arg values. I was skeptical, but it just won't
        // work without this storage buffer.
        // TODO: find out why.
        uint64_t data_buf[argc];

        for (size_t i=0; i<argc; i++) {
                struct InterpValue *arg = &args[i];

                switch (arg->type) {
                case VAL_Num:
                        // FIXME: currently hard converted into int32_t because
                        // some raylib function requires int. I should add the Int
                        // data type along with float.
                        *(int32_t*)&data_buf[i] = (int32_t)arg->f32;
                        ffi_args[i] = &ffi_type_sint32;
                        values[i] = &data_buf[i];
                        break;

                case VAL_Nil:
                        ffi_args[i] = &ffi_type_pointer;
                        values[i] = nullptr;
                        break;

                case VAL_String:
                        data_buf[i] = (uintptr_t)arg->str;
                        ffi_args[i] = &ffi_type_pointer;
                        values[i] = &data_buf[i];
                        break;

                default:
                        ffi_args[i] = &ffi_type_pointer;
                        values[i] = &arg->ptr;
                        break;
                }
        }

        // TODO: currently hard coded to signed 32bit int so
        // that raylib's WindowShouldClose() can work.
        ffi_status status = ffi_prep_cif(
                &cif,
                FFI_DEFAULT_ABI,
                argc,
                &ffi_type_sint32,
                ffi_args
        );

        if (status == FFI_OK) {
                int rc = 0.f;
                ffi_call(&cif, FFI_FN(call_handle.ptr), &rc, values);
                return (struct InterpValue){
                        .type = VAL_Num,
                        .f32 = rc
                };
        }

        return (struct InterpValue){ .type = VAL_Nil };
}

