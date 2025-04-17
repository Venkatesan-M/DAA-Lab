%{
/* LEX specification to count vowels and consonants in a string */
#include <stdio.h>
#include <ctype.h>

int vowels = 0;
int consonants = 0;
%}

%%
[aeiouAEIOU]      { vowels++; }           /* Count vowels */
[b-df-hj-np-tv-zB-DF-HJ-NP-TV-Z]  { consonants++; }  /* Count consonants */
\n                { 
                    printf("Vowels: %d, Consonants: %d\n", vowels, consonants);
                    vowels = 0;
                    consonants = 0;
                  }
.                 /* Ignore other characters */
%%

int main() {
    printf("Enter strings (one per line). Press Ctrl+D to exit:\n");
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}