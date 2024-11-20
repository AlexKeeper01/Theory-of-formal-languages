```
%{
#include <stdio.h>
#include <stdlib.h>
%}

%option noyywrap

%%
"{" { printf("(BEGIN_OBJECT, '{')\n"); }
"}" { printf("(END_OBJECT, '}')\n"); }
"[" { printf("(BEGIN_ARRAY, '[')\n"); }
"]" { printf("(END_ARRAY, ']')\n"); }
"," { printf("(COMMA, ',')\n"); }
":" { printf("(COLON, ':')\n"); }
"true"|"false"|"null" { printf("(LITERAL, '%s')\n", yytext); }
\"[^\"]*\" { printf("(STRING, '%s')\n", yytext); }
[-+]?([0-9]+)([eE][-+]?[0-9]+)? { printf("(NUMBER, %s)\n", yytext); }
[ \t\n]+ ;
. { printf("(UNKNOWN, '%s')\n", yytext); }
%%

int main() {
    yylex();
    return 0;
}
```
