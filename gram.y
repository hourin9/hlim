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

%token <str> ID

%start input
%%
input: stmt_list
     ;

stmt_list: %empty
         | stmt_list stmt
         ;

stmt: ' '
    ;

%%

void yyerror(char const *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

