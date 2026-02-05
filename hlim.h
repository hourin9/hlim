#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

enum ASTType {
        AST_Invalid,
        AST_Asn,
        AST_Decl,
        AST_StringLiteral,
        AST_NumericLiteral,
        AST_Block,
        AST_Id,
        AST_Call,
        AST_Branch,
        AST_Loop,
        AST_Arit,
        AST_Import,
        AST_Include,
};

enum ArtType {
        ART_Add,
        ART_Sub,
        ART_Mul,
        ART_Div,
        ART_Mod,
        ART_Eq,
        ART_Not,
        ART_Neq,

        ART_Index,
};

struct AST {
        enum ASTType type;
        enum ArtType arit;
        float f32;
        bool shallow_copy;

        union {
                char *sval,
                     *id;
        };

        union {
                struct AST *rhs,
                           *body,
                           *cond,
                           *func;
        };

        struct AST *lhs;

        // Linked list of args.
        struct AST *args;
        struct AST *params;

        struct AST *next;
};

void recursive_print(FILE*, const struct AST*,
        int depth, bool follow_next);

struct AST *find_end(struct AST*);

// Appends argument to an arg list.
// Returns new end of the list.
struct AST *append_arg(struct AST *arg, struct AST *argv);

struct AST *default_node();
struct AST *node(enum ASTType, struct AST *args);

// ASTType is not needed as binary operation is implicitly
// assumed to be a number. I created the language, I know.
struct AST *binary(enum ArtType, struct AST *lhs, struct AST *rhs);

struct AST *leaf(enum ASTType, char *sval);
struct AST *number(float);
struct AST *string(char*);
struct AST *id(char *sval);
struct AST *block(struct AST *body);
struct AST *branch(struct AST *cond, struct AST *then,
        struct AST *otherwise);
struct AST *loop(struct AST *cond, struct AST *body);
struct AST *call(struct AST *func, struct AST *argv);

struct AST *shallow_dup(const struct AST*);
struct AST *deep_dup(const struct AST*);

void shallow_del(struct AST*);
void deep_del(struct AST*);

enum ValueType {
        VAL_Nil,
        VAL_Num,
        VAL_String,
        VAL_Id,
        VAL_Node,

        VAL_FFILibHandle,
        VAL_FFISym,
};

struct FFISym {
        void *ptr;

        // Function signature to call FFI funcs.
        // handle_ffi_call() will switch on the chars in this
        // string to cast properly.
        char *sig;
};

int validate_signature(const char*);

struct SSTWrapper;
struct InterpValue {
        enum ValueType type;
        union {
                float f32;
                char *str;
                struct AST *node;

                void *ptr;
                struct FFISym sym;
        };

        struct SSTWrapper *scope;
};

#define NIL_VALUE (struct InterpValue){ .type = VAL_Nil }
#define NUM_VALUE(x) (struct InterpValue){ .type = VAL_Num, .f32 = x }

void print_value(struct InterpValue);

float to_num(struct InterpValue);
bool to_bool(struct InterpValue);

struct AST *to_ast(struct InterpValue);
const char *to_str(struct InterpValue);

typedef struct {
        char *key;
        struct InterpValue value;
} ScopeSymTable_t;

typedef ScopeSymTable_t SST_t;

struct SSTWrapper {
        SST_t *table;
        struct SSTWrapper *parent;
        int ref_count;
};

struct SSTWrapper *new_closure(struct SSTWrapper *par);
void clean_closure(struct SSTWrapper*);
void decref_closure(struct SSTWrapper*);

struct RuntimeSymTable {
        struct SSTWrapper *current;
};

typedef struct RuntimeSymTable RST_t;

RST_t init_runtime_symtable();
void destroy_runtime_symtable(RST_t*);
void rst_new_scope(RST_t*);
void rst_pop_scope(RST_t*);
void rst_closure(RST_t*, struct SSTWrapper*);

SST_t *current_rt_scope(RST_t*);
SST_t *global_rt_scope(RST_t*);
void rst_declare(RST_t*, char *id, struct InterpValue val);
void rst_assign(RST_t*, char *id, struct InterpValue val);
void rst_assign_index(RST_t*, char *id, int index, struct InterpValue);
struct InterpValue rst_find(RST_t*, char *id);

struct InterpValue evaluate_list(RST_t*, const struct AST *root);
struct InterpValue evaluate_one(RST_t*, const struct AST*);
struct InterpValue *evaluate_arg_list(RST_t*, const struct AST*);

struct InterpValue handle_branching(RST_t*, const struct AST*);
struct InterpValue handle_call(RST_t*, const struct AST*);
struct InterpValue handle_decl(RST_t*, const struct AST*);
struct InterpValue handle_asn(RST_t*, const struct AST*);
struct InterpValue handle_loop(RST_t*, const struct AST*);
struct InterpValue handle_arithmetic(RST_t*, const struct AST*);
struct InterpValue handle_indexing(RST_t*, const struct AST*);
struct InterpValue handle_import(RST_t*, const struct AST*);
struct InterpValue handle_include(RST_t*, struct InterpValue);
struct InterpValue handle_ffi_load(
        const struct InterpValue *arg,
        void *handle);
struct InterpValue handle_ffi_call(
        struct InterpValue *args,
        struct InterpValue call_handle);

void push_args_params(RST_t*, const struct AST *params,
        struct InterpValue *argv);
void push_args_simple(RST_t*, struct InterpValue *argv);

// While evaluate_list() evaluates until AST.next is nullptr and
// evaluate_one() does not evaluate block's body, evaluate_block()
// evaluates one node, and if it's a block then the body is evaluated.
//
// While making the thing, I realized this might do more harm than
// good as it'd make the if() evaluates differently from other
// expressions. I prefer predictability.
__attribute__((deprecated))
struct InterpValue evaluate_block(RST_t*, const struct AST *root);

struct InterpValue constant_fold(const struct AST*);
struct InterpValue eliminate_dead_branches(struct AST*);
bool has_side_effect(const struct AST*);
struct AST *optimize(struct AST*);

