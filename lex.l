%{
#include "gram.tab.h"
%}

%option noyywrap

%%

"decl" { return DECL; }

";" { return ';'; }

"{" { return '{'; }
"}" { return '}'; }

"(" { return '('; }
")" { return ')'; }

[a-zA-Z_][a-zA-Z0-9_]* {
        yylval.str = strdup(yytext);
        return ID;
}

[ \t\f\v\n]+ { ; }

%%

