#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_PROD       50
#define MAX_LEN        100
#define MAX_NT         50 
#define MAX_TERM       50
#define MAX_SET_ITEMS  50 
#define MAX_TOKEN_LEN  10

/*******************************
 * Structures and Grammar Code *
 *******************************/

// A production stores a left–hand side and a right–hand side.
typedef struct {
    char lhs[MAX_LEN];
    char rhs[MAX_LEN];
} Production;

// A grammar is a collection of productions.
typedef struct {
    Production productions[MAX_PROD];
    int num_productions;
} Grammar;

void input_grammar(Grammar *g) {
    printf("Enter number of productions: ");
    scanf("%d", &g->num_productions);
    getchar(); 
    printf("Enter productions (format: A->α, use \"e\" for epsilon):\n");
    for (int i = 0; i < g->num_productions; i++) {
        printf("Production %d: ", i + 1);
        char input[MAX_LEN];
        fgets(input, MAX_LEN, stdin);
        input[strcspn(input, "\n")] = '\0';
        // Assume LHS is the first token (up to "->")
        // For simplicity, assume LHS is one or two characters (like A or A')
        int arrowPos = strstr(input, "->") - input;
        strncpy(g->productions[i].lhs, input, arrowPos);
        g->productions[i].lhs[arrowPos] = '\0';
        strcpy(g->productions[i].rhs, input + arrowPos + 2);
    }
}

void print_grammar(const Grammar *g, const char *title) {
    printf("\n%s:\n", title);
    for (int i = 0; i < g->num_productions; i++) {
        printf("%s -> %s\n", g->productions[i].lhs, g->productions[i].rhs);
    }
}

void copy_grammar(Grammar *dest, const Grammar *src) {
    dest->num_productions = src->num_productions;
    for (int i = 0; i < src->num_productions; i++) {
        dest->productions[i] = src->productions[i];
    }
}

/**********************************************
 * Elimination of Immediate Left Recursion     *
 **********************************************/
// Eliminates immediate left recursion for a given nonterminal nt.
// For productions like A -> Aα | β, produces:
//   A  -> β A'
//   A' -> α A' | e
void eliminate_immediate_left_recursion_for_nt(Grammar *g, const char *nt) {
    char new_nt[MAX_LEN];
    sprintf(new_nt, "%s'", nt);

    char nonLeft[MAX_PROD][MAX_LEN] = {0};
    int nonLeftCount = 0;
    char leftRec[MAX_PROD][MAX_LEN] = {0};
    int leftRecCount = 0;
    
    // Partition productions with LHS == nt.
    for (int i = 0; i < g->num_productions; i++) {
        if (strcmp(g->productions[i].lhs, nt) == 0) {
            if (strncmp(g->productions[i].rhs, nt, strlen(nt)) == 0)
                strcpy(leftRec[leftRecCount++], g->productions[i].rhs + strlen(nt));
            else
                strcpy(nonLeft[nonLeftCount++], g->productions[i].rhs);
        }
    }
    if (leftRecCount == 0)
        return; // no left recursion

    // Remove productions for nt.
    int j = 0;
    for (int i = 0; i < g->num_productions; i++) {
        if (strcmp(g->productions[i].lhs, nt) != 0)
            g->productions[j++] = g->productions[i];
    }
    g->num_productions = j;

    // For each non–left–recursive production β, add: nt -> β new_nt
    for (int i = 0; i < nonLeftCount; i++) {
        char new_rhs[MAX_LEN];
        sprintf(new_rhs, "%s%s", nonLeft[i], new_nt);
        // add production: nt -> new_rhs
        strcpy(g->productions[g->num_productions].lhs, nt);
        strcpy(g->productions[g->num_productions].rhs, new_rhs);
        g->num_productions++;
    }
    // For each left–recursive production α, add: new_nt -> α new_nt
    for (int i = 0; i < leftRecCount; i++) {
        char new_rhs[MAX_LEN];
        sprintf(new_rhs, "%s%s", leftRec[i], new_nt);
        strcpy(g->productions[g->num_productions].lhs, new_nt);
        strcpy(g->productions[g->num_productions].rhs, new_rhs);
        g->num_productions++;
    }
    // Finally add: new_nt -> e
    strcpy(g->productions[g->num_productions].lhs, new_nt);
    strcpy(g->productions[g->num_productions].rhs, "e");
    g->num_productions++;
}

/*************************************
 * Simple Left Factoring             *
 *************************************/
void left_factoring(Grammar *g) {
    // For each uppercase letter (or uppercase plus prime) in our productions’ LHS.
    for (int i = 0; i < g->num_productions; i++) {
        char nt[MAX_LEN];
        strcpy(nt, g->productions[i].lhs);
        // Collect all productions with this LHS.
        Production prods[MAX_PROD];
        int count = 0;
        for (int j = 0; j < g->num_productions; j++) {
            if (strcmp(g->productions[j].lhs, nt) == 0)
                prods[count++] = g->productions[j];
        }
        if (count < 2)
            continue;
        // Find common prefix among RHS of these productions.
        char commonPrefix[MAX_LEN];
        strcpy(commonPrefix, prods[0].rhs);
        for (int k = 1; k < count; k++) {
            int j = 0;
            while(commonPrefix[j] && prods[k].rhs[j] && commonPrefix[j]==prods[k].rhs[j])
                j++;
            commonPrefix[j] = '\0';
        }
        if (strlen(commonPrefix) == 0)
            continue;
        bool allSame = true;
        for (int k = 0; k < count; k++) {
            if (strcmp(prods[k].rhs, commonPrefix) != 0) { allSame = false; break; }
        }
        if (allSame)
            continue;
        // Create a new nonterminal for the factored part.
        char new_nt[MAX_LEN];
        sprintf(new_nt, "%s'", nt);
        // Build a new grammar without productions for nt.
        Grammar temp;
        temp.num_productions = 0;
        for (int k = 0; k < g->num_productions; k++) {
            if (strcmp(g->productions[k].lhs, nt) != 0)
                temp.productions[temp.num_productions++] = g->productions[k];
        }
        // Add the factored production: nt -> commonPrefix new_nt
        char new_rhs[MAX_LEN];
        sprintf(new_rhs, "%s%s", commonPrefix, new_nt);
        strcpy(temp.productions[temp.num_productions].lhs, nt);
        strcpy(temp.productions[temp.num_productions].rhs, new_rhs);
        temp.num_productions++;
        // For each production for nt, add production for new_nt.
        for (int k = 0; k < count; k++) {
            char remainder[MAX_LEN];
            if (strlen(prods[k].rhs) > strlen(commonPrefix))
                strcpy(remainder, prods[k].rhs + strlen(commonPrefix));
            else
                strcpy(remainder, "e");
            strcpy(temp.productions[temp.num_productions].lhs, new_nt);
            strcpy(temp.productions[temp.num_productions].rhs, remainder);
            temp.num_productions++;
        }
        copy_grammar(g, &temp);
    }
}

/************************************
 * Nonterminal Collection and Tokenization
 ************************************/

// Global array of nonterminals (each as a string) and count.
char nonterminals[MAX_NT][MAX_LEN];
int nontermCount = 0;

// Adds a nonterminal (if not already present) to the nonterminals array.
void addNonterminal(const char *nt) {
    for (int i = 0; i < nontermCount; i++) {
        if (strcmp(nonterminals[i], nt) == 0)
            return;
    }
    strcpy(nonterminals[nontermCount++], nt);
}

// Returns the index of nonterminal nt.
int getNonterminalIndex(const char *nt) {
    for (int i = 0; i < nontermCount; i++) {
        if (strcmp(nonterminals[i], nt) == 0)
            return i;
    }
    return -1;
}

// Tokenizes a right–hand side string.
// The tokens array (of strings) is filled with tokens; returns the number of tokens.
// A token is defined as: if the current character is uppercase then if the next character is
// a prime ('\''), the token is two‐character long, else one–character; otherwise a single character.
int tokenize(const char *s, char tokens[][MAX_TOKEN_LEN]) {
    int pos = 0, count = 0;
    while (s[pos] != '\0') {
        if (isspace(s[pos])) { pos++; continue; }
        if (isupper(s[pos])) {
            tokens[count][0] = s[pos];
            if (s[pos+1] == '\'') {
                tokens[count][1] = '\'';
                tokens[count][2] = '\0';
                pos += 2;
            } else {
                tokens[count][1] = '\0';
                pos++;
            }
        } else {
            tokens[count][0] = s[pos];
            tokens[count][1] = '\0';
            pos++;
        }
        count++;
    }
    return count;
}


bool isNonterminalToken(const char *token) {
    return isupper(token[0]);
}

/************************************
 * FIRST and FOLLOW Sets Computation *
 ************************************/

// We store FIRST and FOLLOW sets for each nonterminal (by index).
// Each set is an array of strings (each string is a token, e.g., "a", "b", "e" for epsilon).
char FIRSTSET[MAX_NT][MAX_SET_ITEMS][MAX_TOKEN_LEN];
int FIRSTCount[MAX_NT] = {0};

char FOLLOWSET[MAX_NT][MAX_SET_ITEMS][MAX_TOKEN_LEN];
int FOLLOWCount[MAX_NT] = {0};

// Checks whether token is already in the set.
bool tokenInSet(char set[][MAX_TOKEN_LEN], int count, const char *token) {
    for (int i = 0; i < count; i++) {
        if (strcmp(set[i], token) == 0)
            return true;
    }
    return false;
}

// Adds token to set if not already present.
void addTokenToSet(char set[][MAX_TOKEN_LEN], int *count, const char *token) {
    if (!tokenInSet(set, *count, token)) {
        strcpy(set[(*count)++], token);
    }
}

// Returns true if the set contains epsilon (represented by "e").
bool containsEpsilon(char set[][MAX_TOKEN_LEN], int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(set[i], "e") == 0)
            return true;
    }
    return false;
}

// Compute FIRST sets for all nonterminals.
void computeFIRST(const Grammar *g) {
    // First, collect all nonterminals from the grammar.
    nontermCount = 0;
    for (int i = 0; i < g->num_productions; i++) {
        addNonterminal(g->productions[i].lhs);
    }
    // Initialize FIRST sets to empty.
    for (int i = 0; i < nontermCount; i++)
        FIRSTCount[i] = 0;

    bool changed;
    do {
        changed = false;
        // For each production A -> α
        for (int i = 0; i < g->num_productions; i++) {
            int A = getNonterminalIndex(g->productions[i].lhs);
            // Tokenize the RHS.
            char tokens[20][MAX_TOKEN_LEN];
            int ntokens = tokenize(g->productions[i].rhs, tokens);
            if (ntokens == 0) continue;
            bool addEps = true;
            for (int j = 0; j < ntokens && addEps; j++) {
                if (!isNonterminalToken(tokens[j])) {
                    // Terminal: add it.
                    if (!tokenInSet(FIRSTSET[A], FIRSTCount[A], tokens[j])) {
                        addTokenToSet(FIRSTSET[A], &FIRSTCount[A], tokens[j]);
                        changed = true;
                    }
                    addEps = false;
                } else {
                    int B = getNonterminalIndex(tokens[j]);
                    // Add FIRST(B) except epsilon.
                    for (int k = 0; k < FIRSTCount[B]; k++) {
                        if (strcmp(FIRSTSET[B][k], "e") != 0) {
                            if (!tokenInSet(FIRSTSET[A], FIRSTCount[A], FIRSTSET[B][k])) {
                                addTokenToSet(FIRSTSET[A], &FIRSTCount[A], FIRSTSET[B][k]);
                                changed = true;
                            }
                        }
                    }
                    if (!containsEpsilon(FIRSTSET[B], FIRSTCount[B]))
                        addEps = false;
                }
            }
            if (addEps) {
                if (!tokenInSet(FIRSTSET[A], FIRSTCount[A], "e")) {
                    addTokenToSet(FIRSTSET[A], &FIRSTCount[A], "e");
                    changed = true;
                }
            }
        }
    } while(changed);
}

// Compute FOLLOW sets for all nonterminals.
void computeFOLLOW(const Grammar *g) {
    // Initialize FOLLOW sets to empty.
    for (int i = 0; i < nontermCount; i++)
        FOLLOWCount[i] = 0;
    // Add $ to the FOLLOW of the start symbol.
    addTokenToSet(FOLLOWSET[getNonterminalIndex(g->productions[0].lhs)], &FOLLOWCount[getNonterminalIndex(g->productions[0].lhs)], "$");

    bool changed;
    do {
        changed = false;
        for (int i = 0; i < g->num_productions; i++) {
            int A = getNonterminalIndex(g->productions[i].lhs);
            // Tokenize RHS.
            char tokens[20][MAX_TOKEN_LEN];
            int ntokens = tokenize(g->productions[i].rhs, tokens);
            for (int j = 0; j < ntokens; j++) {
                if (isNonterminalToken(tokens[j])) {
                    int B = getNonterminalIndex(tokens[j]);
                    // Compute FIRST(beta) for beta = tokens[j+1...end]
                    bool betaCanEps = true;
                    for (int k = j+1; k < ntokens && betaCanEps; k++) {
                        if (!isNonterminalToken(tokens[k])) {
                            if (!tokenInSet(FOLLOWSET[B], FOLLOWCount[B], tokens[k])) {
                                addTokenToSet(FOLLOWSET[B], &FOLLOWCount[B], tokens[k]);
                                changed = true;
                            }
                            betaCanEps = false;
                        } else {
                            int C = getNonterminalIndex(tokens[k]);
                            for (int m = 0; m < FIRSTCount[C]; m++) {
                                if (strcmp(FIRSTSET[C][m], "e") != 0) {
                                    if (!tokenInSet(FOLLOWSET[B], FOLLOWCount[B], FIRSTSET[C][m])) {
                                        addTokenToSet(FOLLOWSET[B], &FOLLOWCount[B], FIRSTSET[C][m]);
                                        changed = true;
                                    }
                                }
                            }
                            if (!containsEpsilon(FIRSTSET[C], FIRSTCount[C]))
                                betaCanEps = false;
                        }
                    }
                    if (betaCanEps) { // If beta is empty or can produce epsilon, add FOLLOW(A) to FOLLOW(B)
                        for (int m = 0; m < FOLLOWCount[A]; m++) {
                            if (!tokenInSet(FOLLOWSET[B], FOLLOWCount[B], FOLLOWSET[A][m])) {
                                addTokenToSet(FOLLOWSET[B], &FOLLOWCount[B], FOLLOWSET[A][m]);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    } while(changed);
}

/************************************
 * Predictive Parsing Table         *
 ************************************/

// We build the parsing table as a 2D array: rows for nonterminals (by index) and columns for terminals.
// Each cell holds a production (as a string) if defined.
char parsingTable[MAX_NT][MAX_TERM][MAX_LEN];
char terminals[MAX_TERM][MAX_TOKEN_LEN];
int termCount = 0;

// Collect all terminal symbols from the grammar (by scanning RHS tokens).
void computeTerminals(const Grammar *g) {
    termCount = 0;
    char tokens[20][MAX_TOKEN_LEN];
    for (int i = 0; i < g->num_productions; i++) {
        int ntokens = tokenize(g->productions[i].rhs, tokens);
        for (int j = 0; j < ntokens; j++) {
            if (!isNonterminalToken(tokens[j]) && strcmp(tokens[j],"e") != 0) {
                // Check if already added.
                bool found = false;
                for (int k = 0; k < termCount; k++) {
                    if (strcmp(terminals[k], tokens[j]) == 0) { found = true; break; }
                }
                if (!found) {
                    strcpy(terminals[termCount++], tokens[j]);
                }
            }
        }
    }
    // Add end marker "$" if not present.
    bool found = false;
    for (int k = 0; k < termCount; k++) {
        if (strcmp(terminals[k], "$") == 0) { found = true; break; }
    }
    if (!found) {
        strcpy(terminals[termCount++], "$");
    }
}

// For a given RHS string, compute its FIRST set (as a set of tokens) and store in result.
void computeFIRSTForString(const char *rhs, char result[][MAX_TOKEN_LEN], int *resCount) {
    *resCount = 0;
    char tokens[20][MAX_TOKEN_LEN];
    int ntokens = tokenize(rhs, tokens);
    bool addEps = true;
    for (int i = 0; i < ntokens && addEps; i++) {
        if (!isNonterminalToken(tokens[i])) {
            if (!tokenInSet(result, *resCount, tokens[i]))
                addTokenToSet(result, resCount, tokens[i]);
            addEps = false;
        } else {
            int idx = getNonterminalIndex(tokens[i]);
            for (int j = 0; j < FIRSTCount[idx]; j++) {
                if (strcmp(FIRSTSET[idx][j], "e") != 0 && !tokenInSet(result, *resCount, FIRSTSET[idx][j])) {
                    addTokenToSet(result, resCount, FIRSTSET[idx][j]);
                }
            }
            if (!containsEpsilon(FIRSTSET[idx], FIRSTCount[idx]))
                addEps = false;
        }
    }
    if (addEps) {
        addTokenToSet(result, resCount, "e");
    }
}

// Initialize the parsing table.
void initParsingTable() {
    for (int i = 0; i < MAX_NT; i++)
        for (int j = 0; j < MAX_TERM; j++)
            parsingTable[i][j][0] = '\0';
}

// Construct the predictive parsing table.
void constructParsingTable(const Grammar *g) {
    initParsingTable();
    computeTerminals(g);
    char firstTokens[20][MAX_TOKEN_LEN];
    int firstTokenCount;
    // For each production A -> α:
    for (int i = 0; i < g->num_productions; i++) {
        int A = getNonterminalIndex(g->productions[i].lhs);
        computeFIRSTForString(g->productions[i].rhs, firstTokens, &firstTokenCount);
        // For each terminal t in FIRST(α) (except epsilon) add production to table.
        for (int j = 0; j < firstTokenCount; j++) {
            if (strcmp(firstTokens[j], "e") != 0) {
                // Find terminal index for firstTokens[j]
                int col = -1;
                for (int k = 0; k < termCount; k++) {
                    if (strcmp(terminals[k], firstTokens[j]) == 0) {
                        col = k;
                        break;
                    }
                }
                if (col != -1)
                    sprintf(parsingTable[A][col], "%s->%s", g->productions[i].lhs, g->productions[i].rhs);
            }
        }
        // If FIRST(α) contains epsilon, add production for every terminal in FOLLOW(A).
        if (tokenInSet(firstTokens, firstTokenCount, "e")) {
            for (int j = 0; j < FOLLOWCount[A]; j++) {
                // Find terminal index for FOLLOW(A)[j]
                int col = -1;
                for (int k = 0; k < termCount; k++) {
                    if (strcmp(terminals[k], FOLLOWSET[A][j]) == 0) {
                        col = k;
                        break;
                    }
                }
                if (col != -1)
                    sprintf(parsingTable[A][col], "%s->%s", g->productions[i].lhs, g->productions[i].rhs);
            }
        }
    }
}

// Print the parsing table.
void printParsingTable() {
    printf("\nPredictive Parsing Table:\n\n");
    printf("NT/T        ");
    for (int i = 0; i < termCount; i++) {
        printf("%-15s", terminals[i]);
    }
    printf("\n-------------------------------------------------------------\n");
    for (int i = 0; i < nontermCount; i++) {
        // Check if any table entry exists for this nonterminal.
        bool rowExists = false;
        for (int j = 0; j < termCount; j++) {
            if (strlen(parsingTable[i][j]) > 0) { rowExists = true; break; }
        }
        if (rowExists) {
            printf("%-10s", nonterminals[i]);
            for (int j = 0; j < termCount; j++) {
                if (strlen(parsingTable[i][j]) > 0)
                    printf("%-15s", parsingTable[i][j]);
                else
                    printf("%-15s", "-");
            }
            printf("\n");
        }
    }
}

int main() {
    Grammar g = {0}, original;
    input_grammar(&g);
    copy_grammar(&original, &g);
    print_grammar(&original, "Original Grammar");

    // Eliminate left recursion for each nonterminal (try for every uppercase letter and also those with a prime).
    // (We run through our grammar productions and for each distinct LHS, call elimination.)
    char processed[MAX_NT][MAX_LEN];
    int processedCount = 0;
    for (int i = 0; i < g.num_productions; i++) {
        bool already = false;
        for (int j = 0; j < processedCount; j++) {
            if (strcmp(g.productions[i].lhs, processed[j]) == 0) { already = true; break; }
        }
        if (!already) {
            eliminate_immediate_left_recursion_for_nt(&g, g.productions[i].lhs);
            strcpy(processed[processedCount++], g.productions[i].lhs);
        }
    }
    print_grammar(&g, "Grammar after Eliminating Left Recursion");

    // Apply left factoring.
    left_factoring(&g);
    print_grammar(&g, "Final Grammar after Left Factoring");

    // Compute FIRST and FOLLOW sets.
    computeFIRST(&g);
    computeFOLLOW(&g);

    // Print FIRST sets.
    printf("\nFIRST Sets:\n");
    for (int i = 0; i < nontermCount; i++) {
        if (FIRSTCount[i] > 0) {
            printf("FIRST(%s) = { ", nonterminals[i]);
            for (int j = 0; j < FIRSTCount[i]; j++) {
                printf("%s ", FIRSTSET[i][j]);
            }
            printf("}\n");
        }
    }
    // Print FOLLOW sets.
    printf("\nFOLLOW Sets:\n");
    for (int i = 0; i < nontermCount; i++) {
        if (FOLLOWCount[i] > 0) {
            printf("FOLLOW(%s) = { ", nonterminals[i]);
            for (int j = 0; j < FOLLOWCount[i]; j++) {
                printf("%s ", FOLLOWSET[i][j]);
            }
            printf("}\n");
        }
    }

    // Construct and print the predictive parsing table.
    constructParsingTable(&g);
    printParsingTable();

    return 0;
}
