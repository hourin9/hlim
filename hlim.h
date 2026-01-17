#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

enum ASTType {
        AST_Invalid,
        AST_Decl,
        AST_StringLiteral,
        AST_NumericLiteral,
};

struct AST {
        enum ASTType type;
        float f32;
        char *sval;

        // Reserved for arithmetic operations.
        struct AST *rhs, *lhs;

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

