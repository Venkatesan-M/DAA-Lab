%{
/* Minimal LEX specification to check strings starting with vowel */
#include <stdio.h>
char first_char;
int is_first_char;
%}

%%
\n      { 
           is_first_char = 1; 
           /* Print result for empty lines */
           if (first_char == '\0') {
               printf("REJECTED: Empty string\n");
           }
           first_char = '\0';
        }

.       {
           if (is_first_char) {
               first_char = yytext[0];
               is_first_char = 0;
               
               /* Check if first character is a vowel */
               if (first_char == 'a' || first_char == 'e' || 
                   first_char == 'i' || first_char == 'o' || 
                   first_char == 'u' || first_char == 'A' || 
                   first_char == 'E' || first_char == 'I' || 
                   first_char == 'O' || first_char == 'U') {
                   printf("ACCEPTED: String starts with a vowel\n");
               } else {
                   printf("REJECTED: String does not start with a vowel\n");
               }
           }
        }

%%

int main() {
    printf("Enter strings (one per line). Press Ctrl+D to exit:\n");
    is_first_char = 1;
    first_char = '\0';
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}