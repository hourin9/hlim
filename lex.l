%{
#include "gram.tab.h"
%}

%option noyywrap

%%

"decl" { return DECL; }
"if" { return IF; }
"loop" { return LOOP; }

";" { return ';'; }

"{" { return '{'; }
"}" { return '}'; }

"(" { return '('; }
")" { return ')'; }

"+" { return '+'; }
"-" { return '-'; }
"*" { return '*'; }
"/" { return '/'; }

\"[^"\n]*\" {
        yylval.str = strndup(yytext + 1, yyleng - 2);
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

