#include "hlim.h"

#include "external/stb_ds.h"

#include <dlfcn.h>
#include <ffi.h>

int validate_signature(const char *sig)
{
        size_t n = strlen(sig);

        for (size_t i=0; i<n; i++) {
                if (strchr("vifs", sig[i]) == nullptr)
                        return -1;
        }

        return 0;
}

struct InterpValue handle_ffi_load(
        const struct InterpValue *args,
        void *handle)
{
        if (args[0].type != VAL_String)
                return (struct InterpValue){ .type = VAL_Nil };

        void *ptr = dlsym(handle, args[0].str);

        if (!ptr)
                return (struct InterpValue){ .type = VAL_Nil };

        char *sig = (arrlen(args) > 1)
                ? strdup(to_str(args[1]))
                : strdup("i")
                ;

        return (struct InterpValue){
                .type = VAL_FFISym,
                .sym = { ptr, sig }
        };
}

static ffi_type *_push_number(uint64_t *data, float n, char sig)
{
        switch (sig) {
        case 'i':
                *(int32_t*)data = (int32_t)n;
                return &ffi_type_sint32;

        case 'f':
                *(float*)data = n;
                return &ffi_type_float;

        default:
                *(int64_t*)data = (int64_t)n;
                return &ffi_type_sint64;
        }
}

struct InterpValue handle_ffi_call(
        struct InterpValue *args,
        struct InterpValue call_handle)
{
        size_t argc = arrlen(args);
        struct FFISym sym = call_handle.sym;
        size_t sigc = strlen(sym.sig);
        (void)sigc;
        ffi_cif cif;

        ffi_type *ffi_args[argc];
        void *values[argc];

        // AI told me use to use a storage buffer for
        // arg values. I was skeptical, but it just won't
        // work without this storage buffer.
        // TODO: find out why.
        uint64_t data_buf[argc];

        size_t i=0;
        for (i=0; i<argc; i++) {
                struct InterpValue *arg = &args[i];

                switch (arg->type) {
                case VAL_Num:
                        ffi_args[i] = _push_number(
                                &data_buf[i],
                                arg->f32,
                                (i+1 < sigc) ? sym.sig[i+1] : 'i'
                        );
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

        ffi_type *return_type = &ffi_type_void;
        switch (sym.sig[0]) {
        case 'v':
                return_type = &ffi_type_void;
                break;
        case 'f':
                return_type = &ffi_type_float;
                break;
        case 'i':
                return_type = &ffi_type_sint32;
                break;
        case 's':
                return_type = &ffi_type_pointer;
                break;
        }

        ffi_status status = ffi_prep_cif(
                &cif,
                FFI_DEFAULT_ABI,
                argc,
                return_type,
                ffi_args
        );

        if (status != FFI_OK)
                return (struct InterpValue){ .type = VAL_Nil };

        if (return_type == &ffi_type_void) {
                ffi_call(&cif, FFI_FN(call_handle.ptr), nullptr, values);
                return (struct InterpValue){ .type = VAL_Nil };
        }

        ffi_arg result;
        ffi_call(&cif, FFI_FN(call_handle.ptr), &result, values);
        return (struct InterpValue){
                .type = VAL_Num,
                // TODO: cast result to the correct type
                .f32 = (int)result
        };
}

