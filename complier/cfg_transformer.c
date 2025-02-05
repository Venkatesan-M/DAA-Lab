#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_PROD 50
#define MAX_LEN 100

typedef struct {
    char lhs[MAX_LEN];
    char rhs[MAX_LEN];
} Production;

typedef struct {
    Production productions[MAX_PROD];
    int num_productions;
} Grammar;

bool production_exists(Grammar *g, const char *lhs, const char *rhs) {
    for (int i = 0; i < g->num_productions; i++) {
        if (strcmp(g->productions[i].lhs, lhs) == 0 &&
            strcmp(g->productions[i].rhs, rhs) == 0) {
            return true;
        }
    }
    return false;
}

void add_production(Grammar *g, const char *lhs, const char *rhs) {
    if (!production_exists(g, lhs, rhs)) {
        strcpy(g->productions[g->num_productions].lhs, lhs);
        strcpy(g->productions[g->num_productions].rhs, rhs);
        g->num_productions++;
    }
}

// Eliminates immediate left recursion for a given non-terminal (nt).
// nt is a string (e.g., "A") and the new non-terminal will be (e.g., "A'").
void eliminate_immediate_left_recursion_for_nt(Grammar *g, const char *nt) {
    char new_nt[MAX_LEN];
    sprintf(new_nt, "%s'", nt);

    char nonLeft[MAX_PROD][MAX_LEN] = {0};
    int nonLeftCount = 0;
    char leftRec[MAX_PROD][MAX_LEN] = {0}; 
    int leftRecCount = 0;

    for (int i = 0; i < g->num_productions; i++) {
        if (strcmp(g->productions[i].lhs, nt) == 0) {
            if (g->productions[i].rhs[0] == nt[0]) {
                strcpy(leftRec[leftRecCount++], g->productions[i].rhs + 1);
            } else {
                strcpy(nonLeft[nonLeftCount++], g->productions[i].rhs);
            }
        }
    }

    if (leftRecCount == 0)
        return;

    int j = 0;
    for (int i = 0; i < g->num_productions; i++) {
        if (strcmp(g->productions[i].lhs, nt) != 0) {
            g->productions[j++] = g->productions[i];
        }
    }
    g->num_productions = j;

    for (int i = 0; i < nonLeftCount; i++) {
        char new_rhs[MAX_LEN] = {0};
        strcpy(new_rhs, nonLeft[i]);
        strcat(new_rhs, new_nt);
        add_production(g, nt, new_rhs);
    }

    for (int i = 0; i < leftRecCount; i++) {
        char new_rhs[MAX_LEN] = {0};
        strcpy(new_rhs, leftRec[i]);
        strcat(new_rhs, new_nt);
        add_production(g, new_nt, new_rhs);
    }

    add_production(g, new_nt, "ε");
}

void input_grammar(Grammar *g) {
    printf("Enter number of productions: ");
    scanf("%d", &g->num_productions);
    getchar();

    printf("Enter productions in format (A->BC or A->a):\n");
    for (int i = 0; i < g->num_productions; i++) {
        printf("Production %d: ", i + 1);
        char input[MAX_LEN];
        fgets(input, MAX_LEN, stdin);

        g->productions[i].lhs[0] = input[0];
        g->productions[i].lhs[1] = '\0';

        strcpy(g->productions[i].rhs, input + 3);

        int len = strlen(g->productions[i].rhs);
        if (len > 0 && g->productions[i].rhs[len - 1] == '\n')
            g->productions[i].rhs[len - 1] = '\0';
    }
}

void print_grammar(Grammar *g, const char *title) {
    printf("\n%s:\n", title);
    for (int i = 0; i < g->num_productions; i++) {
        printf("%s -> %s\n", g->productions[i].lhs, g->productions[i].rhs);
    }
}

void copy_grammar(Grammar *dest, Grammar *src) {
    dest->num_productions = src->num_productions;
    for (int i = 0; i < src->num_productions; i++) {
        dest->productions[i] = src->productions[i];
    }
}

int main() {
    Grammar g = {0};
    Grammar original = {0};

    input_grammar(&g);
    copy_grammar(&original, &g);

    print_grammar(&original, "Original Grammar");

    for (char c = 'A'; c <= 'Z'; c++) {
        char nt[2] = {c, '\0'};
        eliminate_immediate_left_recursion_for_nt(&g, nt);
    }

    print_grammar(&g, "Grammar after left recursion elimination");

    print_grammar(&g, "Final Grammar after left factoring");

    return 0;
}
