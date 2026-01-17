%{
#include <stdio.h>
#include <stdlib.h>

#include "hlim.h"

extern int yylex();
void yyerror(char const*);

struct AST *parser_ast;
%}

%define parse.error verbose

%union {
        char *str;
        float f32;
        struct AST *node;
}

%token DECL

%token <f32> NUM_LIT
%token <str> ID
%token <str> STR_LIT

%type <node> stmt_list stmt expr
%type <node> decl_body
%type <node> call args_opt arg_list

%start input
%%
input: stmt_list { parser_ast = $1; }
     ;

stmt_list: %empty { $$ = nullptr; }
         | stmt_list stmt { $$ = append_arg($2, $1); }
         ;

stmt: DECL '(' decl_body ')' { $$ = $3; }
    | expr { $$ = $1; }
    ;

decl_body: ID expr { /* TODO */ }
         ;

expr: ID { $$ = nullptr; }
    | NUM_LIT { $$ = number($1); }
    | STR_LIT { $$ = string($1); }
    | '{' stmt_list '}' { /* TODO */ }
    | call
    ;

call: expr '(' args_opt ')'
    ;

args_opt: %empty { $$ = nullptr; }
        | arg_list
        ;

arg_list: arg_list expr { /* TODO */ }
        | expr
        ;

%%

void yyerror(char const *msg)
{
        fprintf(stderr, "%s\n", msg);
        exit(1);
}

