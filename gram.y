%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex();
void yyerror(char const*);
%}

%define parse.error verbose

%union {
        char *str;
        float f32;
}

%token DECL

%token <f32> NUM_LIT
%token <str> ID
%token <str> STR_LIT

%start input
%%
input: stmt_list
     ;

stmt_list: %empty
         | stmt_list stmt
         ;

stmt: DECL '(' decl_body ')'
    | expr
    ;

decl_body: ID expr
         ;

expr: ID
    | NUM_LIT
    | STR_LIT
    | '{' stmt_list '}'
    | call
    ;

call: expr '(' args_opt ')'
    ;

args_opt: %empty
        | arg_list
        ;

arg_list: arg_list expr
        | expr
        ;

%%

void yyerror(char const *msg)
{
        fprintf(stderr, "%s\n", msg);
        exit(1);
}

