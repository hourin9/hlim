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

%type <node> stmt_list stmt expr block binary_operation primary
%type <node> decl_body asn_body params_opt id_list
%type <node> call args_opt arg_list
%type <node> if_body loop_body
%type <node> pipeline
%type <node> list_index

%left ARROW
%left '+' '-'
%left '*' '/' '%'
%left EQ NEQ
%left ':'
%left '('
%nonassoc NOT

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

id_list: ID { $$ = id($1); }
       | id_list ID {
                struct AST *newid = id($2);
                append_arg(newid, $1);
                $$ = $1;
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

primary: NUM_LIT { $$ = number($1); }
       | STR_LIT { $$ = string($1); }
       | ID { $$ = id($1); }
       | block { $$ = $1; }
       | '[' expr ']' { $$ = $2; }
       ;

expr: DECL '(' decl_body ')' { $$ = $3; }
    | ASN '(' asn_body ')' { $$ = $3; }
    | binary_operation { $$ = $1; }
    | NOT expr { $$ = binary(ART_Not, $2, nullptr); }
    | IF '(' if_body ')' { $$ = $3; }
    | pipeline { $$ = $1; }
    | call { $$ = $1; }
    ;

list_index: primary { $$ = $1; }
          | list_index ':' primary { $$ = binary(ART_Index, $1, $3); }
          ;

binary_operation: expr '+' expr { $$ = binary(ART_Add, $1, $3); }
                | expr '-' expr { $$ = binary(ART_Sub, $1, $3); }
                | expr '*' expr { $$ = binary(ART_Mul, $1, $3); }
                | expr '/' expr { $$ = binary(ART_Div, $1, $3); }
                | expr '%' expr { $$ = binary(ART_Mod, $1, $3); }
                | expr EQ expr { $$ = binary(ART_Eq, $1, $3); }
                | expr NEQ expr { $$ = binary(ART_Neq, $1, $3); }
                ;

block: params_opt '{' stmt_list '}' {
        $$ = block($3);
        $$->params = $1;
     }
     ;

params_opt: %empty { $$ = nullptr; }
          | '<' id_list '>' { $$ = $2; }
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

call: list_index { $$ = $1; }
    | call '(' args_opt ')' { $$ = call($1, $3); }
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

pipeline: expr ARROW call {
                struct AST *call = $3;
                struct AST *args = call->args;
                call->args = $1;
                call->args->next = args;
                $$ = call;
        }
        ;

%%

void yyerror(char const *msg)
{
        fprintf(stderr, "%s\n", msg);
        exit(1);
}

