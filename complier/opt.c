#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_PRODUCTIONS 10
#define MAX_TERMINALS 10
#define MAX_SIZE 50
#define MAX_NON_TERMINALS 10

typedef struct {
    char nonTerminal;
    char production[MAX_SIZE];  // Right-hand side of the production
} Grammar;

Grammar grammar[MAX_PRODUCTIONS];
int productionCount;

char terminals[MAX_TERMINALS];
int terminalCount = 0;

char nonTerminals[MAX_NON_TERMINALS];
int nonTerminalCount = 0;

// We use FIRSTVT and LASTVT arrays (for operator precedence)
char firstVT[MAX_NON_TERMINALS][MAX_TERMINALS];
char lastVT[MAX_NON_TERMINALS][MAX_TERMINALS];

char precedenceTable[MAX_TERMINALS][MAX_TERMINALS];

// Function to check if a symbol is a non-terminal (uppercase letter)
int isNonTerminal(char symbol) {
    return (symbol >= 'A' && symbol <= 'Z');
}

// Function to check if a symbol is a terminal
// (for our purpose, we assume that if a symbol is not uppercase, it is an operator/token)
int isTerminal(char symbol) {
    return (!isNonTerminal(symbol) && symbol != '\0' && symbol != '$' && symbol != '#');
}

// Add a terminal if not already present
void addTerminal(char terminal) {
    for (int i = 0; i < terminalCount; i++) {
        if (terminals[i] == terminal)
            return; // already added
    }
    terminals[terminalCount++] = terminal;
}

// Add a non-terminal if not already present
void addNonTerminal(char nonTerminal) {
    for (int i = 0; i < nonTerminalCount; i++) {
        if (nonTerminals[i] == nonTerminal)
            return; // already added
    }
    nonTerminals[nonTerminalCount++] = nonTerminal;
}

// First, collect all non-terminals from the LHS of productions.
// Then, when scanning RHS, add a symbol as terminal only if it is not an uppercase letter.
void identifySymbols() {
    // First pass: add non-terminals from LHS.
    for (int i = 0; i < productionCount; i++) {
        addNonTerminal(grammar[i].nonTerminal);
    }
    // Second pass: scan RHS and add only if the symbol is not uppercase.
    for (int i = 0; i < productionCount; i++) {
        char *rhs = grammar[i].production;
        for (int j = 0; rhs[j] != '\0'; j++) {
            if (!isNonTerminal(rhs[j])) {
                addTerminal(rhs[j]);
            }
        }
    }
    // Add $ as the end-marker.
    addTerminal('$');
}

// Initialize the precedence table with blanks.
void initializePrecedenceTable() {
    for (int i = 0; i < terminalCount; i++) {
        for (int j = 0; j < terminalCount; j++) {
            precedenceTable[i][j] = ' ';
        }
    }
}

// Find index of a terminal in the terminals array.
int getTerminalIndex(char terminal) {
    for (int i = 0; i < terminalCount; i++) {
        if (terminals[i] == terminal)
            return i;
    }
    return -1;
}

// Find index of a non-terminal in the nonTerminals array.
int getNonTerminalIndex(char nonTerminal) {
    for (int i = 0; i < nonTerminalCount; i++) {
        if (nonTerminals[i] == nonTerminal)
            return i;
    }
    return -1;
}

// Compute FIRSTVT sets for non-terminals (only those on LHS).
void computeFirstVT() {
    // Initialize to 0.
    for (int i = 0; i < nonTerminalCount; i++)
        for (int j = 0; j < terminalCount; j++)
            firstVT[i][j] = 0;

    int changed = 1;
    while(changed) {
        changed = 0;
        for (int i = 0; i < productionCount; i++) {
            int A = getNonTerminalIndex(grammar[i].nonTerminal);
            char *rhs = grammar[i].production;
            int len = strlen(rhs);
            if (len == 0) continue;
            // If the first symbol is terminal (and is in our terminals set), add it.
            if (!isNonTerminal(rhs[0]) && getTerminalIndex(rhs[0]) != -1) {
                int t = getTerminalIndex(rhs[0]);
                if (!firstVT[A][t]) {
                    firstVT[A][t] = 1;
                    changed = 1;
                }
            }
            // If the first symbol is non-terminal.
            if (isNonTerminal(rhs[0])) {
                // If there is a symbol following and it is terminal.
                if (len > 1 && !isNonTerminal(rhs[1]) && getTerminalIndex(rhs[1]) != -1) {
                    int t = getTerminalIndex(rhs[1]);
                    if (!firstVT[A][t]) {
                        firstVT[A][t] = 1;
                        changed = 1;
                    }
                }
                // Add FIRSTVT of the first non-terminal.
                int B = getNonTerminalIndex(rhs[0]);
                if (B != -1) {
                    for (int t = 0; t < terminalCount; t++) {
                        if (firstVT[B][t] && !firstVT[A][t]) {
                            firstVT[A][t] = 1;
                            changed = 1;
                        }
                    }
                }
            }
        }
    }
}

// Compute LASTVT sets for non-terminals.
void computeLastVT() {
    // Initialize to 0.
    for (int i = 0; i < nonTerminalCount; i++)
        for (int j = 0; j < terminalCount; j++)
            lastVT[i][j] = 0;
    int changed = 1;
    while(changed) {
        changed = 0;
        for (int i = 0; i < productionCount; i++) {
            int A = getNonTerminalIndex(grammar[i].nonTerminal);
            char *rhs = grammar[i].production;
            int len = strlen(rhs);
            if (len == 0) continue;
            // If the last symbol is terminal.
            if (!isNonTerminal(rhs[len-1]) && getTerminalIndex(rhs[len-1]) != -1) {
                int t = getTerminalIndex(rhs[len-1]);
                if (!lastVT[A][t]) {
                    lastVT[A][t] = 1;
                    changed = 1;
                }
            }
            // If the last symbol is non-terminal and the symbol before it is terminal.
            if (len >= 2 && isNonTerminal(rhs[len-1]) && !isNonTerminal(rhs[len-2]) && getTerminalIndex(rhs[len-2]) != -1) {
                int t = getTerminalIndex(rhs[len-2]);
                if (!lastVT[A][t]) {
                    lastVT[A][t] = 1;
                    changed = 1;
                }
            }
            // If the last symbol is non-terminal, add its LASTVT set.
            if (isNonTerminal(rhs[len-1])) {
                int B = getNonTerminalIndex(rhs[len-1]);
                if (B != -1) {
                    for (int t = 0; t < terminalCount; t++) {
                        if (lastVT[B][t] && !lastVT[A][t]) {
                            lastVT[A][t] = 1;
                            changed = 1;
                        }
                    }
                }
            }
        }
    }
}

void fillPrecedenceTable() {
    computeFirstVT();
    computeLastVT();
    
    initializePrecedenceTable();
    
    for (int i = 0; i < productionCount; i++) {
        char *rhs = grammar[i].production;
        int len = strlen(rhs);
        for (int j = 0; j < len - 1; j++) {
            char a = rhs[j], b = rhs[j+1];
            // Rule 1: if both a and b are terminals.
            if (!isNonTerminal(a) && !isNonTerminal(b) && getTerminalIndex(a) != -1 && getTerminalIndex(b) != -1) {
                int idx_a = getTerminalIndex(a);
                int idx_b = getTerminalIndex(b);
                precedenceTable[idx_a][idx_b] = '=';
            }
            // Rule 2: if a is terminal and b is non-terminal.
            if (!isNonTerminal(a) && isNonTerminal(b)) {
                int idx_a = getTerminalIndex(a);
                int B = getNonTerminalIndex(b);
                if (B != -1) {
                    for (int k = 0; k < terminalCount; k++) {
                        if (firstVT[B][k])
                            precedenceTable[idx_a][k] = '<';
                    }
                }
            }
            // Rule 3: if a is non-terminal and b is terminal.
            if (isNonTerminal(a) && !isNonTerminal(b)) {
                int idx_b = getTerminalIndex(b);
                int A = getNonTerminalIndex(a);
                if (A != -1) {
                    for (int k = 0; k < terminalCount; k++) {
                        if (lastVT[A][k])
                            precedenceTable[k][idx_b] = '>';
                    }
                }
            }
            // Additional rule: if there is a pattern terminal, non-terminal, terminal.
            if (j < len - 2) {
                char c = rhs[j+2];
                if (!isNonTerminal(a) && isNonTerminal(b) && !isNonTerminal(c) &&
                    getTerminalIndex(a) != -1 && getTerminalIndex(c) != -1) {
                    int idx_a = getTerminalIndex(a);
                    int idx_c = getTerminalIndex(c);
                    precedenceTable[idx_a][idx_c] = '=';
                }
            }
        }
    }
    
    int dollarIndex = getTerminalIndex('$');
    for (int i = 0; i < terminalCount; i++) {
        if (i != dollarIndex) {
            precedenceTable[dollarIndex][i] = '<';
            precedenceTable[i][dollarIndex] = '>';
        }
    }
    
    for (int i = 0; i < terminalCount; i++) {
        if (terminals[i] != '$')
            precedenceTable[i][i] = '<';
    }
}

// Print the operator precedence table.
void printPrecedenceTable() {
    printf("\nOperator Precedence Table:\n\n");
    
    // Print header row.
    printf("  | ");
    for (int i = 0; i < terminalCount; i++) {
        printf(" %c ", terminals[i]);
    }
    printf("\n");
    
    printf("--+-");
    for (int i = 0; i < terminalCount; i++) {
        printf("---");
    }
    printf("\n");
    
    for (int i = 0; i < terminalCount; i++) {
        printf(" %c | ", terminals[i]);
        for (int j = 0; j < terminalCount; j++) {
            printf(" %c ", precedenceTable[i][j]);
        }
        printf("\n");
    }
}

int main() {
    printf("Enter the number of productions: ");
    scanf("%d", &productionCount);
    
    for (int i = 0; i < productionCount; i++) {
        char input[MAX_SIZE];
        printf("Enter production %d (format A->α): ", i + 1);
        scanf("%s", input);
        
        if (input[1] != '-' || input[2] != '>') {
            printf("Invalid format! Use A->α format.\n");
            i--;
            continue;
        }
        
        grammar[i].nonTerminal = input[0];
        strcpy(grammar[i].production, input + 3);  // Skip "->"
    }
    
    identifySymbols();
    
    printf("\nTerminals identified: { ");
    for (int i = 0; i < terminalCount; i++) {
        printf("%c ", terminals[i]);
    }
    printf("}\n");
    
    initializePrecedenceTable();
    fillPrecedenceTable();
    
    printPrecedenceTable();
    
    return 0;
}
