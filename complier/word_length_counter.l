%{
/* LEX specification to count words with length between 7 and 9 characters */
#include <stdio.h>
int count = 0;  /* Counter for words with length 7-9 */
%}

%%
[a-zA-Z0-9_]{7,9}  { 
    /* Count words with length between 7 and 9 characters */
    count++; 
}

[a-zA-Z0-9_]+  { 
    /* Ignore other words */ 
}

[ \t,.:;!?()]+  { 
    /* Ignore whitespace and punctuation */ 
}

\n  { 
    /* When newline encountered, print count and reset */
    printf("Number of words with length between 7 and 9: %d\n", count);
    count = 0;
}

%%

int main() {
    printf("Enter strings (one per line). Press Ctrl+D to exit:\n");
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}