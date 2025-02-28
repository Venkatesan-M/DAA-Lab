#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTIONS 10
#define SIZE 50

typedef struct {
    char nonTerminal;
    char production[SIZE];  // Right-hand side of the production
    char leading[SIZE];     // Computed LEADING set
    char trailing[SIZE];    // Computed TRAILING set
} Grammar;

Grammar grammar[MAX_PRODUCTIONS];
int productionCount;

int isNonTerminal(char symbol) {
    return (symbol >= 'A' && symbol <= 'Z');
}

int isTerminal(char symbol) {
    return (!isNonTerminal(symbol) && symbol != '\0');
}

void addSymbolToSet(char *set, char symbol) {
    if (symbol == '\0')
        return;
    int len = strlen(set);
    for (int i = 0; i < len; i++) {
        if (set[i] == symbol)
            return; // already present
    }
    set[len] = symbol;
    set[len + 1] = '\0';
}

// Function to compute the LEADING sets iteratively
void computeLeadingSets() {
    int changed = 1;
    while (changed) {
        changed = 0;
        // For each production A -> α
        for (int i = 0; i < productionCount; i++) {
            char *rhs = grammar[i].production;
            // If the production is nonempty, examine its first symbol
            if (rhs[0] != '\0') {
                // Case 1: If the first symbol is a terminal, add it to LEADING(A)
                if (isTerminal(rhs[0])) {
                    int oldLen = strlen(grammar[i].leading);
                    addSymbolToSet(grammar[i].leading, rhs[0]);
                    if (strlen(grammar[i].leading) > oldLen)
                        changed = 1;
                }
                // Case 2: If the first symbol is a non-terminal, add all symbols in its LEADING set.
                else if (isNonTerminal(rhs[0])) {
                    // Find the production(s) for this non-terminal.
                    for (int j = 0; j < productionCount; j++) {
                        if (grammar[j].nonTerminal == rhs[0]) {
                            int oldLen = strlen(grammar[i].leading);
                            for (int k = 0; k < strlen(grammar[j].leading); k++) {
                                addSymbolToSet(grammar[i].leading, grammar[j].leading[k]);
                            }
                            if (strlen(grammar[i].leading) > oldLen)
                                changed = 1;
                        }
                    }
                    // Additionally, if a terminal immediately follows the non-terminal in the production, add it.
                    if (rhs[1] != '\0' && isTerminal(rhs[1])) {
                        int oldLen = strlen(grammar[i].leading);
                        addSymbolToSet(grammar[i].leading, rhs[1]);
                        if (strlen(grammar[i].leading) > oldLen)
                            changed = 1;
                    }
                }
            }
        }
    }
}

// Function to compute the TRAILING sets iteratively
void computeTrailingSets() {
    int changed = 1;
    while (changed) {
        changed = 0;
        // For each production A -> α
        for (int i = 0; i < productionCount; i++) {
            char *rhs = grammar[i].production;
            int len = strlen(rhs);
            if (len > 0) {
                // Case 1: If the last symbol is a terminal, add it to TRAILING(A)
                char last = rhs[len - 1];
                if (isTerminal(last)) {
                    int oldLen = strlen(grammar[i].trailing);
                    addSymbolToSet(grammar[i].trailing, last);
                    if (strlen(grammar[i].trailing) > oldLen)
                        changed = 1;
                }
                // Case 2: If the last symbol is a non-terminal, add all symbols in its TRAILING set.
                else if (isNonTerminal(last)) {
                    for (int j = 0; j < productionCount; j++) {
                        if (grammar[j].nonTerminal == last) {
                            int oldLen = strlen(grammar[i].trailing);
                            for (int k = 0; k < strlen(grammar[j].trailing); k++) {
                                addSymbolToSet(grammar[i].trailing, grammar[j].trailing[k]);
                            }
                            if (strlen(grammar[i].trailing) > oldLen)
                                changed = 1;
                        }
                    }
                    // Also, if a terminal immediately precedes the non-terminal, add it.
                    if (len - 2 >= 0 && isTerminal(rhs[len - 2])) {
                        int oldLen = strlen(grammar[i].trailing);
                        addSymbolToSet(grammar[i].trailing, rhs[len - 2]);
                        if (strlen(grammar[i].trailing) > oldLen)
                            changed = 1;
                    }
                }
            }
        }
    }
}

int main() {
    printf("Enter the number of productions: ");
    scanf("%d", &productionCount);
    
    for (int i = 0; i < productionCount; i++) {
        char input[SIZE];
        printf("Enter production %d:", i + 1);
        scanf("%s", input);
        grammar[i].nonTerminal = input[0];
        strcpy(grammar[i].production, input + 2);
        grammar[i].leading[0] = '\0';
        grammar[i].trailing[0] = '\0';
    }
    
    computeLeadingSets();
    computeTrailingSets();
    
    printf("\nLEADING and TRAILING sets:\n");
    for (int i = 0; i < productionCount; i++) {
        printf("\nFor production %c -> %s:\n", grammar[i].nonTerminal, grammar[i].production);
        printf("LEADING(%c) = { %s }\n", grammar[i].nonTerminal, grammar[i].leading);
        printf("TRAILING(%c) = { %s }\n", grammar[i].nonTerminal, grammar[i].trailing);
    }
    
    return 0;
}
