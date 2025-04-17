%{
/* LEX specification to find the length of a string */
#include <stdio.h>
int length = 0;
%}

%%
. { length++; } /* Count each character */
\n { printf("Length of the string: %d\n", length); length = 0; } /* Print length and reset counter for new lines */
%%

int main() {
    printf("Enter strings (one per line). Press Ctrl+D to exit:\n");
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}