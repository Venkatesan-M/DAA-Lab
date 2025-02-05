#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PROD 50
#define MAX_LEN 50

typedef struct {
    char lhs;
    char rhs[MAX_LEN];
} Production;

void computeFirst(Production *prod, int n, char *first[], int *firstLen);
void computeFollow(Production *prod, int n, char *first[], int *firstLen, char *follow[], int *followLen);
int isNonTerminal(char c);
void addToSet(char *set, int *len, char c);
void printSet(char *set, int len);
void printNonTerminalSets(Production *prod, int n, char *sets[], int *setLen, const char *setName);
int hasEpsilon(char *set, int len);

int main() {
    Production prod[MAX_PROD];
    char *first[26];
    char *follow[26];
    int firstLen[26] = {0};
    int followLen[26] = {0};
    int n, i;

    for(i = 0; i < 26; i++) {
        first[i] = (char*)malloc(MAX_LEN * sizeof(char));
        follow[i] = (char*)malloc(MAX_LEN * sizeof(char));
        memset(first[i], 0, MAX_LEN);
        memset(follow[i], 0, MAX_LEN);
    }

    printf("Enter number of productions: ");
    scanf("%d", &n);
    getchar();

    printf("Enter productions (format A->B):\n");
    for(i = 0; i < n; i++) {
        char input[MAX_LEN];
        fgets(input, MAX_LEN, stdin);
        prod[i].lhs = input[0];
        strcpy(prod[i].rhs, input + 3);
        prod[i].rhs[strcspn(prod[i].rhs, "\n")] = 0;
    }

    computeFirst(prod, n, first, firstLen);
    computeFollow(prod, n, first, firstLen, follow, followLen);

    printf("\nFIRST sets:\n");
    printNonTerminalSets(prod, n, first, firstLen, "FIRST");
    
    printf("\nFOLLOW sets:\n");
    printNonTerminalSets(prod, n, follow, followLen, "FOLLOW");

    for(i = 0; i < 26; i++) {
        free(first[i]);
        free(follow[i]);
    }

    return 0;
}
void computeFirst(Production *prod, int n, char *first[], int *firstLen) {
    int changes;
    do {
        changes = 0;
        for (int i = 0; i < n; i++) {
            char nt = prod[i].lhs;
            int oldLen = firstLen[nt - 'A'];
            
            // Case: Production is epsilon.
            if (strcmp(prod[i].rhs, "e") == 0) {
                addToSet(first[nt - 'A'], &firstLen[nt - 'A'], 'e');
            } else {
                int j = 0;
                int addEpsilon = 1;
                while (prod[i].rhs[j] != '\0' && addEpsilon) {
                    char symbol = prod[i].rhs[j];
                    if (!isNonTerminal(symbol)) {
                        // Terminal: add it and mark that epsilon is not produced.
                        addToSet(first[nt - 'A'], &firstLen[nt - 'A'], symbol);
                        addEpsilon = 0;
                    } else {
                        // Non-terminal: add all symbols from its FIRST (except epsilon)
                        for (int k = 0; k < firstLen[symbol - 'A']; k++) {
                            if (first[symbol - 'A'][k] != 'e')
                                addToSet(first[nt - 'A'], &firstLen[nt - 'A'], first[symbol - 'A'][k]);
                        }
                        // If the non-terminal does not produce epsilon, then stop.
                        if (!hasEpsilon(first[symbol - 'A'], firstLen[symbol - 'A']))
                            addEpsilon = 0;
                    }
                    j++;
                }
                if (addEpsilon)
                    addToSet(first[nt - 'A'], &firstLen[nt - 'A'], 'e');
            }
            if (firstLen[nt - 'A'] > oldLen)
                changes = 1;
        }
    } while (changes);
}


void computeFollow(Production *prod, int n, char *first[], int *firstLen, char *follow[], int *followLen) {
    // Add $ to the FOLLOW set of the start symbol
    addToSet(follow[prod[0].lhs - 'A'], &followLen[prod[0].lhs - 'A'], '$');
    
    int changes;
    do {
        changes = 0;
        for (int i = 0; i < n; i++) {
            char lhs = prod[i].lhs;
            for (int j = 0; prod[i].rhs[j] != '\0'; j++) {
                if (!isNonTerminal(prod[i].rhs[j]))
                    continue;
                int oldLen = followLen[prod[i].rhs[j] - 'A'];
                char current = prod[i].rhs[j];
                // For production A -> αBβ, add FIRST(β) - {e} to FOLLOW(B)
                if (prod[i].rhs[j + 1] != '\0') {
                    if (!isNonTerminal(prod[i].rhs[j + 1])) {
                        addToSet(follow[current - 'A'], &followLen[current - 'A'], prod[i].rhs[j + 1]);
                    } else {
                        char next = prod[i].rhs[j + 1];
                        for (int k = 0; k < firstLen[next - 'A']; k++) {
                            if (first[next - 'A'][k] != 'e')
                                addToSet(follow[current - 'A'], &followLen[current - 'A'], first[next - 'A'][k]);
                        }
                    }
                }
                // For A -> αB or A -> αBβ where FIRST(β) contains epsilon,
                // add FOLLOW(A) to FOLLOW(B)
                if (prod[i].rhs[j + 1] == '\0' ||
                    (isNonTerminal(prod[i].rhs[j + 1]) &&
                     hasEpsilon(first[prod[i].rhs[j + 1] - 'A'], firstLen[prod[i].rhs[j + 1] - 'A']))) {
                    for (int k = 0; k < followLen[lhs - 'A']; k++) {
                        addToSet(follow[current - 'A'], &followLen[current - 'A'], follow[lhs - 'A'][k]);
                    }
                }
                if (followLen[prod[i].rhs[j] - 'A'] > oldLen)
                    changes = 1;
            }
        }
    } while (changes);
}

int isNonTerminal(char c) {
    return c >= 'A' && c <= 'Z';
}

void addToSet(char *set, int *len, char c) {
    for (int i = 0; i < *len; i++) {
        if (set[i] == c)
            return;
    }
    set[*len] = c;
    (*len)++;
}

void printSet(char *set, int len) {
    for (int i = 0; i < len; i++) {
        printf("%c ", set[i]);
    }
}

void printNonTerminalSets(Production *prod, int n, char *sets[], int *setLen, const char *setName) {
    int printed[26] = {0};
    for (int i = 0; i < n; i++) {
        char nt = prod[i].lhs;
        if (isNonTerminal(nt) && !printed[nt - 'A']) {
            printf("%s(%c) = { ", setName, nt);
            printSet(sets[nt - 'A'], setLen[nt - 'A']);
            printf("}\n");
            printed[nt - 'A'] = 1;
        }
    }
}

int hasEpsilon(char *set, int len) {
    for (int i = 0; i < len; i++) {
        if (set[i] == 'e')
            return 1;
    }
    return 0;
}
