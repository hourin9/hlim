%{
#include "gram.tab.h"

// Consumes "\n" and return the '\n' (newline character).
static char *_process_str(char *raw)
{
        char *dest = raw;
        char *src = raw;
        while (*src) {
                if (*src == '\\' && *(src + 1)) {
                        src ++;
                        switch (*src) {
                        case 'n':
                                *dest++ = '\n';
                                break;
                        case 't':
                                *dest++ = '\t';
                                break;
                        case 'r':
                                *dest++ = '\r';
                                break;
                        case '\\':
                                *dest++ = '\\';
                                break;
                        default:
                                *dest++ = *src;
                                break;
                        }
                } else
                        *dest++ = *src;
                src++;
        }

        *dest = '\0';
        return raw;
}

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
        char *raw = strndup(yytext + 1, yyleng - 2);
        yylval.str = _process_str(raw);
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

"//"[^\n]* { ; }

"/*"([^*]|\*+[^*/])*(\*+)"/" { ; }

[ \t\f\v\n]+ { ; }

%%

