%{
/* LEX specification to count the number of words in a string */
#include <stdio.h>
int word_count = 0;
int in_word = 0;  /* Flag to track if we're currently in a word */
%}

%%
[a-zA-Z0-9_]+ { 
    /* Count each continuous sequence of alphanumeric characters as one word */
    word_count++; 
}

[ \t]+ { 
    /* Ignore whitespace */ 
}

\n      { 
    /* When a newline is encountered, print the count and reset */ 
    printf("Number of words: %d\n", word_count); 
    word_count = 0; 
}

[^a-zA-Z0-9_\n\t ]+ { 
    /* Handle special characters and punctuation as word separators */ 
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