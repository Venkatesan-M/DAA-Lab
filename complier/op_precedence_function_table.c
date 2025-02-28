#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_PRODUCTIONS 10
#define MAX_TERMINALS 20
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

// FIRSTVT and LASTVT sets for non-terminals (indexed by nonTerminal and terminal)
char firstVT[MAX_NON_TERMINALS][MAX_TERMINALS];
char lastVT[MAX_NON_TERMINALS][MAX_TERMINALS];

// Precedence relation table: each cell holds '<', '=', '>', or blank.
char precedenceTable[MAX_TERMINALS][MAX_TERMINALS];

// Arrays to hold operator precedence function values.
int fVal[MAX_TERMINALS];
int gVal[MAX_TERMINALS];

// Check if a symbol is a non-terminal (assumed uppercase)
int isNonTerminal(char symbol) {
    return (symbol >= 'A' && symbol <= 'Z');
}

// Check if a symbol is a terminal (for our purpose, any symbol that is not uppercase)
int isTerminal(char symbol) {
    return (!isNonTerminal(symbol) && symbol != '\0');
}

// Add a terminal if not already present.
void addTerminal(char terminal) {
    for (int i = 0; i < terminalCount; i++) {
        if (terminals[i] == terminal)
            return;
    }
    terminals[terminalCount++] = terminal;
}

// Add a non-terminal if not already present.
void addNonTerminal(char nonTerminal) {
    for (int i = 0; i < nonTerminalCount; i++) {
        if (nonTerminals[i] == nonTerminal)
            return;
    }
    nonTerminals[nonTerminalCount++] = nonTerminal;
}

// Identify symbols in the grammar.
// Non-terminals come from the left-hand side of productions.
// In the right-hand side, any symbol that is not uppercase is treated as a terminal.
void identifySymbols() {
    // First, add LHS non-terminals.
    for (int i = 0; i < productionCount; i++) {
        addNonTerminal(grammar[i].nonTerminal);
    }
    // Then, scan the RHS to add terminals.
    for (int i = 0; i < productionCount; i++) {
        char *rhs = grammar[i].production;
        for (int j = 0; rhs[j] != '\0'; j++) {
            if (!isNonTerminal(rhs[j])) {
                addTerminal(rhs[j]);
            }
        }
    }
    // Finally, add the end-marker.
    addTerminal('$');
}

// Initialize the precedence table with blank spaces.
void initializePrecedenceTable() {
    for (int i = 0; i < terminalCount; i++) {
        for (int j = 0; j < terminalCount; j++) {
            precedenceTable[i][j] = ' ';
        }
    }
}

// Get index of a terminal in the terminals array.
int getTerminalIndex(char terminal) {
    for (int i = 0; i < terminalCount; i++) {
        if (terminals[i] == terminal)
            return i;
    }
    return -1;
}

// Get index of a non-terminal in the nonTerminals array.
int getNonTerminalIndex(char nonTerminal) {
    for (int i = 0; i < nonTerminalCount; i++) {
        if (nonTerminals[i] == nonTerminal)
            return i;
    }
    return -1;
}

// Compute FIRSTVT sets for non-terminals.
void computeFirstVT() {
    // Initialize FIRSTVT sets.
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
            // Rule 1: If production A -> a... where a is terminal.
            if (!isNonTerminal(rhs[0])) {
                int t = getTerminalIndex(rhs[0]);
                if (t != -1 && !firstVT[A][t]) {
                    firstVT[A][t] = 1;
                    changed = 1;
                }
            }
            // Rule 2: If production A -> B a... where B is non-terminal and a is terminal.
            if (isNonTerminal(rhs[0]) && len > 1 && !isNonTerminal(rhs[1])) {
                int t = getTerminalIndex(rhs[1]);
                if (t != -1 && !firstVT[A][t]) {
                    firstVT[A][t] = 1;
                    changed = 1;
                }
            }
            // Rule 3: If production A -> B..., add FIRSTVT(B) to FIRSTVT(A).
            if (isNonTerminal(rhs[0])) {
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
    // Initialize LASTVT sets.
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
            // Rule 1: If production A -> ...a where a is terminal.
            if (!isNonTerminal(rhs[len-1])) {
                int t = getTerminalIndex(rhs[len-1]);
                if (t != -1 && !lastVT[A][t]) {
                    lastVT[A][t] = 1;
                    changed = 1;
                }
            }
            // Rule 2: If production A -> ...aB where a is terminal and B is non-terminal.
            if (len >= 2 && isNonTerminal(rhs[len-1]) && !isNonTerminal(rhs[len-2])) {
                int t = getTerminalIndex(rhs[len-2]);
                if (t != -1 && !lastVT[A][t]) {
                    lastVT[A][t] = 1;
                    changed = 1;
                }
            }
            // Rule 3: If production A -> ...B where B is non-terminal, add LASTVT(B) to LASTVT(A).
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

// Fill the precedence relation table using the FIRSTVT and LASTVT sets.
void fillPrecedenceTable() {
    computeFirstVT();
    computeLastVT();
    initializePrecedenceTable();
    
    // For each production, examine adjacent symbols.
    for (int i = 0; i < productionCount; i++) {
        char *rhs = grammar[i].production;
        int len = strlen(rhs);
        for (int j = 0; j < len - 1; j++) {
            char a = rhs[j], b = rhs[j+1];
            // Rule 1: If both symbols are terminals, assign a = b.
            if (isTerminal(a) && isTerminal(b)) {
                int idx_a = getTerminalIndex(a);
                int idx_b = getTerminalIndex(b);
                if (idx_a != -1 && idx_b != -1)
                    precedenceTable[idx_a][idx_b] = '=';
            }
            // Rule 2: If a is terminal and b is non-terminal,
            // then for every terminal x in FIRSTVT(b), assign a < x.
            if (isTerminal(a) && isNonTerminal(b)) {
                int idx_a = getTerminalIndex(a);
                int B = getNonTerminalIndex(b);
                if (B != -1) {
                    for (int k = 0; k < terminalCount; k++) {
                        if (firstVT[B][k])
                            precedenceTable[idx_a][k] = '<';
                    }
                }
            }
            // Rule 3: If a is non-terminal and b is terminal,
            // then for every terminal x in LASTVT(a), assign x > b.
            if (isNonTerminal(a) && isTerminal(b)) {
                int idx_b = getTerminalIndex(b);
                int A = getNonTerminalIndex(a);
                if (A != -1) {
                    for (int k = 0; k < terminalCount; k++) {
                        if (lastVT[A][k])
                            precedenceTable[k][idx_b] = '>';
                    }
                }
            }
            // Additional pattern: terminal, non-terminal, terminal → assign first and third symbols '='.
            if (j < len - 2) {
                char c = rhs[j+2];
                if (isTerminal(a) && isNonTerminal(b) && isTerminal(c)) {
                    int idx_a = getTerminalIndex(a);
                    int idx_c = getTerminalIndex(c);
                    if (idx_a != -1 && idx_c != -1)
                        precedenceTable[idx_a][idx_c] = '=';
                }
            }
        }
    }
    
    // Special handling for the end-marker '$'.
    int dollarIndex = getTerminalIndex('$');
    for (int i = 0; i < terminalCount; i++) {
        if (i != dollarIndex) {
            precedenceTable[dollarIndex][i] = '<';
            precedenceTable[i][dollarIndex] = '>';
        }
    }
    
    // Optionally, set the relation for the same operator (except '$') to '<'.
    for (int i = 0; i < terminalCount; i++) {
        if (terminals[i] != '$')
            precedenceTable[i][i] = '<';
    }
}

// Compute operator precedence functions f(·) and g(·) from the relation table.
// We require that for terminals a and b:
//    if precedenceTable[a][b] is '<', then f(a) < g(b);
//    if '=', then f(a) == g(b);
//    if '>', then f(a) > g(b).
// A simple iterative relaxation is used.
void computePrecedenceFunctions() {
    // Initialize all function values to 0.
    for (int i = 0; i < terminalCount; i++) {
        fVal[i] = 0;
        gVal[i] = 0;
    }
    // For the end-marker '$', we keep f($)=g($)=0.
    int dollarIndex = getTerminalIndex('$');
    if (dollarIndex != -1) {
        fVal[dollarIndex] = 0;
        gVal[dollarIndex] = 0;
    }
    
    int changed = 1;
    while(changed) {
        changed = 0;
        for (int i = 0; i < terminalCount; i++) {
            for (int j = 0; j < terminalCount; j++) {
                char rel = precedenceTable[i][j];
                if (rel == '<') {
                    // We require: f(a) < g(b)
                    if (fVal[i] >= gVal[j]) {
                        gVal[j] = fVal[i] + 1;
                        changed = 1;
                    }
                } else if (rel == '>') {
                    // We require: f(a) > g(b)
                    if (fVal[i] <= gVal[j]) {
                        fVal[i] = gVal[j] + 1;
                        changed = 1;
                    }
                } else if (rel == '=') {
                    // Equality: force both to the maximum of the two.
                    int newVal = fVal[i] > gVal[j] ? fVal[i] : gVal[j];
                    if (fVal[i] != newVal || gVal[j] != newVal) {
                        fVal[i] = newVal;
                        gVal[j] = newVal;
                        changed = 1;
                    }
                }
            }
        }
    }
}

// Print the operator precedence relation table.
void printPrecedenceTable() {
    printf("\nOperator Precedence Relation Table:\n\n");
    // Header row.
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
    // Table rows.
    for (int i = 0; i < terminalCount; i++) {
        printf(" %c | ", terminals[i]);
        for (int j = 0; j < terminalCount; j++) {
            printf(" %c ", precedenceTable[i][j]);
        }
        printf("\n");
    }
}

// Print the operator precedence function table.
void printPrecedenceFunctionTable() {
    printf("\nOperator Precedence Function Table:\n\n");
    printf("Terminal\tf()\tg()\n");
    for (int i = 0; i < terminalCount; i++) {
        printf("   %c\t\t%d\t%d\n", terminals[i], fVal[i], gVal[i]);
    }
}

int main() {
    printf("Enter the number of productions: ");
    scanf("%d", &productionCount);
    
    for (int i = 0; i < productionCount; i++) {
        char input[MAX_SIZE];
        printf("Enter production %d (format A->α): ", i + 1);
        scanf("%s", input);
        // Check the format A->α.
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
    
    fillPrecedenceTable();
    printPrecedenceTable();
    
    computePrecedenceFunctions();
    printPrecedenceFunctionTable();
    
    return 0;
}
