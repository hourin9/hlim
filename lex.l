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

"asn" { return ASN; }
"decl" { return DECL; }
"drop" { return DROP; }
"if" { return IF; }
"loop" { return LOOP; }
"optimize" { return OPTIMIZE; }
"import" { return IMPORT; }
"include" { return INCLUDE; }

"->" { return ARROW; }

";" { return ';'; }

"{" { return '{'; }
"}" { return '}'; }

"(" { return '('; }
")" { return ')'; }

"[" { return '['; }
"]" { return ']'; }

":" { return ':'; }

"+" { return '+'; }
"-" { return '-'; }
"*" { return '*'; }
"/" { return '/'; }
"%" { return '%'; }

"<" { return '<'; }
">" { return '>'; }

"==" { return EQ; }
"!=" { return NEQ; }
"!" { return NOT; }

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

"0x"[a-zA-Z0-9]+ {
        yylval.f32 = (int)strtol(yytext, NULL, 16);
        return NUM_LIT;
}

"//"[^\n]* { ; }

"/*"([^*]|\*+[^*/])*(\*+)"/" { ; }

[ \t\f\v\n]+ { ; }

%%

