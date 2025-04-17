%{
/* LEX specification to determine if a number is an integer or float */
#include <stdio.h>
%}

%%
^[+-]?[0-9]+$                  { printf("INTEGER\n"); }
^[+-]?[0-9]*\.[0-9]+$          { printf("FLOAT\n"); }
^[+-]?[0-9]+\.[0-9]*$          { printf("FLOAT\n"); }
^[+-]?[0-9]+(\.[0-9]+)?[eE][+-]?[0-9]+$  { printf("FLOAT (Scientific Notation)\n"); }
^.+$                           { printf("NOT A VALID NUMBER\n"); }
\n                             { /* ignore newlines */ }
%%

int main() {
    printf("Enter numbers (one per line). Press Ctrl+D to exit:\n");
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}