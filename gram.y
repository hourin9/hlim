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

%token ASN
%token DECL
%token EQ
%token IF
%token LOOP
%token NEQ
%token NOT

%token <f32> NUM_LIT
%token <str> ID
%token <str> STR_LIT

%type <node> stmt_list stmt expr block binary_operation
%type <node> decl_body asn_body
%type <node> call args_opt arg_list
%type <node> if_body
%type <node> loop_body

%left '+' '-'
%left '*' '/' '%'
%left EQ NEQ NOT
%left '('

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

stmt: LOOP '(' loop_body ')' { $$ = $3; }
    | expr { $$ = $1; }
    ;

decl_body: ID expr {
                struct AST *args = string($1);
                args->next = $2;
                $$ = node(AST_Decl, args);
         }
         ;

asn_body: ID expr {
                struct AST *args = string($1);
                args->next = $2;
                $$ = node(AST_Asn, args);
        }
        ;

expr: ID { $$ = id($1); }
    | DECL '(' decl_body ')' { $$ = $3; }
    | ASN '(' asn_body ')' { $$ = $3; }
    | NUM_LIT { $$ = number($1); }
    | binary_operation { $$ = $1; }
    | NOT expr { $$ = binary(ART_Not, $2, nullptr); }
    | STR_LIT { $$ = string($1); }
    | block { $$ = $1; }
    | IF '(' if_body ')' { $$ = $3; }
    | call
    ;

binary_operation: expr '+' expr { $$ = binary(ART_Add, $1, $3); }
                | expr '-' expr { $$ = binary(ART_Sub, $1, $3); }
                | expr '*' expr { $$ = binary(ART_Mul, $1, $3); }
                | expr '/' expr { $$ = binary(ART_Div, $1, $3); }
                | expr '%' expr { $$ = binary(ART_Mod, $1, $3); }
                | expr EQ expr { $$ = binary(ART_Eq, $1, $3); }
                | expr NEQ expr { $$ = binary(ART_Neq, $1, $3); }
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

arg_list: arg_list expr {
                append_arg($2, $1);
                $$ = $1;
        }
        | expr { $$ = $1; }
        ;

%%

void yyerror(char const *msg)
{
        fprintf(stderr, "%s\n", msg);
        exit(1);
}

