#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_PRODUCTIONS 50
#define MAX_LEN 50
#define MAX_NONTERM 26
#define MAX_TERM 50

/*******************************
 * Production and Grammar Data *
 *******************************/
typedef struct {
    char lhs;
    char rhs[MAX_LEN];
} Production;

Production productions[MAX_PRODUCTIONS];
int prodCount = 0;

/*******************************
 * FIRST and FOLLOW Data       *
 *******************************/
char firstSet[MAX_NONTERM][MAX_TERM];
char followSet[MAX_NONTERM][MAX_TERM];
int firstCount[MAX_NONTERM] = {0};
int followCount[MAX_NONTERM] = {0};

bool inSet(char *set, int count, char symbol) {
    for (int i = 0; i < count; i++) {
        if (set[i] == symbol)
            return true;
    }
    return false;
}

void addToSet(char *set, int *count, char symbol) {
    if (!inSet(set, *count, symbol)) {
        set[(*count)++] = symbol;
    }
}

int nontermIndex(char nt) {
    return nt - 'A';
}


void inputGrammar() {
    printf("Enter number of productions: ");
    scanf("%d", &prodCount);
    getchar();
    for (int i = 0; i < prodCount; i++) {
        printf("Production %d: ", i+1);
        char input[MAX_LEN];
        fgets(input, MAX_LEN, stdin);
        input[strcspn(input, "\n")] = '\0';
        productions[i].lhs = input[0];
        strcpy(productions[i].rhs, input + 3);
    }
}

void printGrammar(const char *title) {
    printf("\n%s:\n", title);
    for (int i = 0; i < prodCount; i++) {
        printf("%c -> %s\n", productions[i].lhs, productions[i].rhs);
    }
}

void printFirstSets() {
    printf("\nFIRST Sets:\n");
    for (char nt = 'A'; nt <= 'Z'; nt++) {
        int idx = nontermIndex(nt);
        if (firstCount[idx] > 0) {
            printf("FIRST(%c) = { ", nt);
            for (int j = 0; j < firstCount[idx]; j++)
                printf("%c ", firstSet[idx][j]);
            printf("}\n");
        }
    }
}

void printFollowSets() {
    printf("\nFOLLOW Sets:\n");
    for (char nt = 'A'; nt <= 'Z'; nt++) {
        int idx = nontermIndex(nt);
        if (followCount[idx] > 0) {
            printf("FOLLOW(%c) = { ", nt);
            for (int j = 0; j < followCount[idx]; j++)
                printf("%c ", followSet[idx][j]);
            printf("}\n");
        }
    }
}

/*******************************
 * Left Recursion Elimination  *
 *******************************/
// For a given nonterminal nt, eliminate immediate left recursion.
// For productions A -> Aα | β, create:
//    A  -> βA'
//    A' -> αA' | e
void eliminateImmediateLeftRecursionFor(char nt) {
    char new_nt = nt + 1; // For simplicity, choose the next letter as A'. (In practice, use a prime.)
    char nonLeft[MAX_PRODUCTIONS][MAX_LEN] = {0};
    int nonLeftCount = 0;
    char leftRec[MAX_PRODUCTIONS][MAX_LEN] = {0};
    int leftRecCount = 0;
    
    for (int i = 0; i < prodCount; i++) {
        if (productions[i].lhs == nt) {
            if (productions[i].rhs[0] == nt) {
                strcpy(leftRec[leftRecCount++], productions[i].rhs + 1);
            } else {
                strcpy(nonLeft[nonLeftCount++], productions[i].rhs);
            }
        }
    }
    if (leftRecCount == 0)
        return;

    Production newProds[MAX_PRODUCTIONS];
    int newCount = 0;
    for (int i = 0; i < prodCount; i++) {
        if (productions[i].lhs != nt)
            newProds[newCount++] = productions[i];
    }
    // For each β in nonLeft, add production: nt -> β new_nt
    for (int i = 0; i < nonLeftCount; i++) {
        Production p;
        p.lhs = nt;
        sprintf(p.rhs, "%s%c", nonLeft[i], new_nt);
        newProds[newCount++] = p;
    }
    // For each α in leftRec, add production: new_nt -> α new_nt
    for (int i = 0; i < leftRecCount; i++) {
        Production p;
        p.lhs = new_nt;
        sprintf(p.rhs, "%s%c", leftRec[i], new_nt);
        newProds[newCount++] = p;
    }
    // Finally, add production: new_nt -> e
    {
        Production p;
        p.lhs = new_nt;
        strcpy(p.rhs, "e");
        newProds[newCount++] = p;
    }
    prodCount = newCount;
    for (int i = 0; i < prodCount; i++) {
        productions[i] = newProds[i];
    }
}

void eliminateLeftRecursion() {
    for (char nt = 'A'; nt <= 'Z'; nt++) {
        eliminateImmediateLeftRecursionFor(nt);
    }
}

/*******************************
 * FIRST and FOLLOW Computation*
 *******************************/
void computeFirstOfString(const char *alpha, char *result, int *resCount) {
    *resCount = 0;
    if (alpha[0] == '\0')
        return;
    bool deriveEps = true;
    for (int i = 0; alpha[i] != '\0' && deriveEps; i++) {
        char symbol = alpha[i];
        if (!isupper(symbol)) {
            addToSet(result, resCount, symbol);
            deriveEps = false;
        } else {
            int idx = nontermIndex(symbol);
            for (int j = 0; j < firstCount[idx]; j++) {
                if (firstSet[idx][j] != 'e')
                    addToSet(result, resCount, firstSet[idx][j]);
            }
            if (!inSet(firstSet[idx], firstCount[idx], 'e'))
                deriveEps = false;
        }
    }
    if (deriveEps)
        addToSet(result, resCount, 'e');
}

void computeFIRST() {
    for (int i = 0; i < MAX_NONTERM; i++)
        firstCount[i] = 0;
    bool changed;
    do {
        changed = false;
        for (int i = 0; i < prodCount; i++) {
            int idx = nontermIndex(productions[i].lhs);
            int oldCount = firstCount[idx];
            char temp[MAX_TERM] = "";
            int tempCount = 0;
            computeFirstOfString(productions[i].rhs, temp, &tempCount);
            for (int j = 0; j < tempCount; j++) {
                if (!inSet(firstSet[idx], firstCount[idx], temp[j])) {
                    addToSet(firstSet[idx], &firstCount[idx], temp[j]);
                }
            }
            if (firstCount[idx] > oldCount)
                changed = true;
        }
    } while(changed);
}

void computeFOLLOW() {
    for (int i = 0; i < MAX_NONTERM; i++)
        followCount[i] = 0;
    // Assume the start symbol is productions[0].lhs; add '$' to its FOLLOW.
    int start = nontermIndex(productions[0].lhs);
    addToSet(followSet[start], &followCount[start], '$');

    bool changed;
    do {
        changed = false;
        for (int i = 0; i < prodCount; i++) {
            char A = productions[i].lhs;
            int Aidx = nontermIndex(A);
            int len = strlen(productions[i].rhs);
            for (int pos = 0; pos < len; pos++) {
                char symbol = productions[i].rhs[pos];
                if (!isupper(symbol))
                    continue;
                int B = nontermIndex(symbol);
                int oldCount = followCount[B];
                char beta[MAX_LEN] = "";
                if (pos + 1 < len)
                    strcpy(beta, productions[i].rhs + pos + 1);
                char firstBeta[MAX_TERM] = "";
                int firstBetaCount = 0;
                computeFirstOfString(beta, firstBeta, &firstBetaCount);
                for (int j = 0; j < firstBetaCount; j++) {
                    if (firstBeta[j] != 'e')
                        addToSet(followSet[B], &followCount[B], firstBeta[j]);
                }
                if (strlen(beta) == 0 || inSet(firstBeta, firstBetaCount, 'e')) {
                    for (int j = 0; j < followCount[Aidx]; j++)
                        addToSet(followSet[B], &followCount[B], followSet[Aidx][j]);
                }
                if (followCount[B] > oldCount)
                    changed = true;
            }
        }
    } while(changed);
}

/*******************************
 * LL(1) Conflict Checking      *
 *******************************/
bool intersect(char *set1, int count1, char *set2, int count2) {
    for (int i = 0; i < count1; i++)
        if (inSet(set2, count2, set1[i]))
            return true;
    return false;
}

bool isLL1() {
    bool ll1 = true;
    for (char A = 'A'; A <= 'Z'; A++) {
        Production prodsForA[MAX_PRODUCTIONS];
        int countA = 0;
        for (int i = 0; i < prodCount; i++) {
            if (productions[i].lhs == A) {
                prodsForA[countA++] = productions[i];
            }
        }
        if (countA <= 1)
            continue;
        for (int i = 0; i < countA; i++) {
            char firstI[MAX_TERM] = "";
            int firstICount = 0;
            computeFirstOfString(prodsForA[i].rhs, firstI, &firstICount);
            for (int j = i+1; j < countA; j++) {
                char firstJ[MAX_TERM] = "";
                int firstJCount = 0;
                computeFirstOfString(prodsForA[j].rhs, firstJ, &firstJCount);
                if (intersect(firstI, firstICount, firstJ, firstJCount)) {
                    printf("Conflict for nonterminal %c: FIRST(%s) and FIRST(%s) intersect.\n", 
                           A, prodsForA[i].rhs, prodsForA[j].rhs);
                    ll1 = false;
                }
                int Aidx = nontermIndex(A);
                if (inSet(firstI, firstICount, 'e')) {
                    if (intersect(firstJ, firstJCount, followSet[Aidx], followCount[Aidx])) {
                        printf("Conflict for nonterminal %c: FIRST(%s) contains e and FIRST(%s) intersects FOLLOW(%c).\n", 
                               A, prodsForA[i].rhs, prodsForA[j].rhs, A);
                        ll1 = false;
                    }
                }
                if (inSet(firstJ, firstJCount, 'e')) {
                    if (intersect(firstI, firstICount, followSet[Aidx], followCount[Aidx])) {
                        printf("Conflict for nonterminal %c: FIRST(%s) contains e and FIRST(%s) intersects FOLLOW(%c).\n", 
                               A, prodsForA[j].rhs, prodsForA[i].rhs, A);
                        ll1 = false;
                    }
                }
            }
        }
    }
    return ll1;
}

int main() {
    // Step 1: Input Grammar.
    inputGrammar();
    printGrammar("Original Grammar");

    // Step 2: Preprocess the grammar by eliminating left recursion.
    eliminateLeftRecursion();
    printGrammar("Grammar after Eliminating Left Recursion");

    // Step 3: Compute FIRST and FOLLOW sets.
    computeFIRST();
    computeFOLLOW();
    printFirstSets();
    printFollowSets();

    // Step 4: Check if grammar is LL(1).
    if (isLL1())
        printf("\nThe grammar is LL(1).\n");
    else
        printf("\nThe grammar is NOT LL(1).\n");

    return 0;
}
