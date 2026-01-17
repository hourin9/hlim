%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex();
void yyerror(char const*);
%}

%define parse.error verbose

%union {
        char *str;
}

%token DECL

%token <str> ID

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
    | '{' stmt_list '}'
    ;

%%

void yyerror(char const *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

