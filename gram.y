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
%token IF
%token LOOP

%token <f32> NUM_LIT
%token <str> ID
%token <str> STR_LIT

%type <node> stmt_list stmt expr block
%type <node> decl_body
%type <node> call args_opt arg_list
%type <node> if_body
%type <node> loop_body

%start input
%%
input: stmt_list { parser_ast = $1; }
     ;

stmt_list: %empty { $$ = nullptr; }
         | stmt_list stmt {
                if ($1 == nullptr)
                        $$ = $2;
                else {
                        append_arg($2, $1);
                        $$ = $1;
                }
         }
         ;

stmt: DECL '(' decl_body ')' { $$ = $3; }
    | IF '(' if_body ')' { $$ = $3; }
    | LOOP '(' loop_body ')' { $$ = $3; }
    | expr { $$ = $1; }
    ;

decl_body: ID expr {
                struct AST *args = string($1);
                args->next = $2;
                $$ = node(AST_Decl, args);
         }
         ;

expr: ID { $$ = id($1); }
    | NUM_LIT { $$ = number($1); }
    | STR_LIT { $$ = string($1); }
    | block { $$ = $1; }
    | call
    ;

block: '{' stmt_list '}' { $$ = block($2); }
     ;

if_body: expr expr expr {
                $$ = branch($1, $2, $3);
       }
       | expr expr {
                $$ = branch($1, $2, nullptr);
       }
       ;

loop_body: expr expr {
                $$ = loop($1, $2);
         }
         ;

call: expr '(' args_opt ')' {
        $$ = node(AST_Call, $3);
        $$->func = $1;
    }
    ;

args_opt: %empty { $$ = nullptr; }
        | arg_list { $$ = $1; }
        ;

arg_list: arg_list expr { $1->next = $2; $$ = $1; }
        | expr { $$ = $1; }
        ;

%%

void yyerror(char const *msg)
{
        fprintf(stderr, "%s\n", msg);
        exit(1);
}

