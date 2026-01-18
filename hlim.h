#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

enum ASTType {
        AST_Invalid,
        AST_Decl,
        AST_StringLiteral,
        AST_NumericLiteral,
        AST_Block,
        AST_Id,
        AST_Call,
        AST_Branch,
};

struct AST {
        enum ASTType type;
        float f32;

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

        // Reserved for arithmetic operations.
        struct AST *lhs;

        // Linked list of args.
        struct AST *args;

        struct AST *next;
};

void recursive_print(FILE*, const struct AST*, int depth);

struct AST *find_end(struct AST*);

// Appends argument to an arg list.
// Returns new end of the list.
struct AST *append_arg(struct AST *arg, struct AST *argv);

struct AST *default_node();
struct AST *node(enum ASTType, struct AST *args);

// ASTType is not needed as binary operation is implicitly
// assumed to be a number. I created the language, I know.
struct AST *binary(struct AST *lhs, struct AST *rhs);

struct AST *leaf(enum ASTType, char *sval);
struct AST *number(float);
struct AST *string(char*);
struct AST *id(char *sval);
struct AST *block(struct AST *body);
struct AST *branch(struct AST *cond, struct AST *then,
        struct AST *otherwise);

enum ValueType {
        VAL_Nil,
        VAL_Num,
        VAL_String,
        VAL_Id,
        VAL_Node,
};

struct InterpValue {
        enum ValueType type;
        union {
                float f32;
                char *str;
                struct AST *node;
        };
};

typedef struct {char *key; struct InterpValue value;} ScopeSymTable_t;
typedef ScopeSymTable_t SST_t;

struct RuntimeSymTable {
        SST_t **levels;
        size_t current;
};

typedef struct RuntimeSymTable RST_t;

RST_t init_runtime_symtable();
void rst_new_scope(RST_t*);

SST_t *current_rt_scope(RST_t*);
SST_t *global_rt_scope(RST_t*);
void rst_set(RST_t*, char *id, struct InterpValue val);
struct InterpValue rst_find_one_scope(RST_t*, char *id, size_t scope);

void print_value(struct InterpValue);
bool to_bool(struct InterpValue);
struct InterpValue evaluate_list(RST_t*, const struct AST *root);
struct InterpValue evaluate_one(RST_t*, const struct AST*);

