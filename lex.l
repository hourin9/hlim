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

\".*\" {
        yylval.str = strdup(yytext);
        return STR_LIT;
}

[0-9]+(\.[0-9]+)? {
        yylval.f32 = atof(yytext);
        return NUM_LIT;
}

[a-zA-Z_][a-zA-Z0-9_]* {
        yylval.str = strdup(yytext);
        return ID;
}

[ \t\f\v\n]+ { ; }

%%

