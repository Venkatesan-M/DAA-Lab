#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PROD 20
#define MAX_LEN 100
#define MAX_DERIVATIONS 50

// Structure to store a production rule
typedef struct {
    char lhs;
    char rhs[MAX_LEN];
} Production;

// Structure to store the grammar
typedef struct {
    Production productions[MAX_PROD];
    int num_productions;
    char terminals[MAX_LEN];
    char non_terminals[MAX_LEN];
} Grammar;

void input_grammar(Grammar *g) {
    printf("\nEnter number of productions: ");
    scanf("%d", &g->num_productions);
    getchar();

    printf("\nEnter productions in format (A->BC or A->a):\n");
    for(int i = 0; i < g->num_productions; i++) {
        printf("Production %d: ", i+1);
        char input[MAX_LEN];
        fgets(input, MAX_LEN, stdin);
        
        g->productions[i].lhs = input[0];
        strcpy(g->productions[i].rhs, input + 3); // Skip "->"
        
        int len = strlen(g->productions[i].rhs);
        if(g->productions[i].rhs[len-1] == '\n')
            g->productions[i].rhs[len-1] = '\0';
    }
}

int generate_derivations(Grammar *g, char *current, char *target, 
                        char derivations[MAX_DERIVATIONS][MAX_LEN], int *count) {
    if(*count >= MAX_DERIVATIONS)
        return 0;
        
    if(strcmp(current, target) == 0) {
        strcpy(derivations[*count], current);
        (*count)++;
        return 1;
    }
    
    for(int i = 0; i < strlen(current); i++) {
        if(isupper(current[i])) { // Non-terminal found
            for(int j = 0; j < g->num_productions; j++) {
                if(g->productions[j].lhs == current[i]) {
                    char new_str[MAX_LEN];
                    strncpy(new_str, current, i);
                    new_str[i] = '\0';
                    strcat(new_str, g->productions[j].rhs);
                    strcat(new_str, current + i + 1);
                    
                    generate_derivations(g, new_str, target, derivations, count);
                }
            }
            break; // Only process leftmost non-terminal
        }
    }
    return 0;
}

bool is_ambiguous(Grammar *g) {
    char test_strings[MAX_DERIVATIONS][MAX_LEN];
    int num_strings = 0;
    
    for(int i = 0; i < g->num_productions; i++) {
        if(g->productions[i].lhs == 'S') {
            strcpy(test_strings[num_strings++], g->productions[i].rhs);
        }
    }
    
    for(int i = 0; i < num_strings; i++) {
        char derivations[MAX_DERIVATIONS][MAX_LEN];
        int count = 0;
        
        generate_derivations(g, "S", test_strings[i], derivations, &count);
        
        if(count > 1) {
            printf("\nAmbiguous string found: %s\n", test_strings[i]);
            printf("Multiple derivations exist:\n");
            for(int j = 0; j < count; j++) {
                printf("Derivation %d: %s\n", j+1, derivations[j]);
            }
            return true;
        }
    }
    
    
    return false;
}

int main() {
    Grammar g;
    printf("Context-Free Grammar Ambiguity Checker\n");
    printf("======================================\n");
    
    input_grammar(&g);
    
    if(is_ambiguous(&g)) {
        printf("\nThe grammar is AMBIGUOUS.\n");
    } else {
        printf("\nNo ambiguity detected for simple strings.\n");
        printf("Note: The grammar might still be ambiguous for longer strings.\n");
    }
    
    return 0;
}