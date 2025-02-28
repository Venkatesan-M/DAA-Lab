    #include <stdio.h>
    #include <string.h>
    #include <ctype.h>
    #include <stdlib.h>

    #define MAX_PRODUCTIONS 20
    #define MAX_TERMINALS 20
    #define MAX_NON_TERMINALS 10
    #define MAX_SIZE 100
    #define MAX_STACK_SIZE 100

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

    char precedenceTable[MAX_TERMINALS][MAX_TERMINALS];

    char stack[MAX_STACK_SIZE];
    int top = -1;

    // Stack operations
    int isStackEmpty() {
        return top == -1;
    }

    void push(char symbol) {
        if (top >= MAX_STACK_SIZE - 1) {
            printf("Stack overflow\n");
            exit(1);
        }
        stack[++top] = symbol;
    }

    char pop() {
        if (isStackEmpty()) {
            printf("Stack underflow\n");
            exit(1);
        }
        return stack[top--];
    }

    // Symbol classification
    // A symbol is a non-terminal if it appears in the nonTerminals array.
    int isNonTerminal(char symbol) {
        for (int i = 0; i < nonTerminalCount; i++) {
            if (nonTerminals[i] == symbol)
                return 1;
        }
        return 0;
    }

    // A symbol is a terminal if it appears in the terminals array.
    int isTerminal(char symbol) {
        for (int i = 0; i < terminalCount; i++) {
            if (terminals[i] == symbol)
                return 1;
        }
        return 0;
    }

    // Add a terminal symbol if not already added.
    void addTerminal(char terminal) {
        if (terminal == ' ' || terminal == '\n') return;
        for (int i = 0; i < terminalCount; i++) {
            if (terminals[i] == terminal)
                return;
        }
        terminals[terminalCount++] = terminal;
    }

    // Add a non-terminal symbol if not already added.
    void addNonTerminal(char nonTerminal) {
        for (int i = 0; i < nonTerminalCount; i++) {
            if (nonTerminals[i] == nonTerminal)
                return;
        }
        nonTerminals[nonTerminalCount++] = nonTerminal;
    }

    // Identify all terminals and non-terminals in the grammar.
    void identifySymbols() {
        // Add LHS non-terminals from productions.
        for (int i = 0; i < productionCount; i++) {
            addNonTerminal(grammar[i].nonTerminal);
        }
        
        // Scan each production’s RHS.
        for (int i = 0; i < productionCount; i++) {
            char *rhs = grammar[i].production;
            for (int j = 0; rhs[j] != '\0'; j++) {
                // Assume non-terminals are uppercase letters.
                if (isalpha(rhs[j]) && isupper(rhs[j])) {
                    addNonTerminal(rhs[j]);
                } else if (rhs[j] != ' ') {
                    addTerminal(rhs[j]);
                }
            }
        }
        
        // Add end marker.
        addTerminal('$');
    }

    // Returns the index of a terminal in the terminals array.
    int getTerminalIndex(char terminal) {
        for (int i = 0; i < terminalCount; i++) {
            if (terminals[i] == terminal)
                return i;
        }
        return -1;
    }

    // Set up the precedence table manually based on the grammar.       
    void setupPrecedenceTable() {
        // Initialize table with blank spaces.
        for (int i = 0; i < terminalCount; i++) {
            for (int j = 0; j < terminalCount; j++) {
                precedenceTable[i][j] = ' ';
            }
        }
        
        int plus_idx   = getTerminalIndex('+');
        int mul_idx    = getTerminalIndex('*');
        int lparen_idx = getTerminalIndex('(');
        int rparen_idx = getTerminalIndex(')');
        int id_idx     = getTerminalIndex('i');
        int dollar_idx = getTerminalIndex('$');
        
        // For '+' operator.
        if (plus_idx != -1) {
            precedenceTable[plus_idx][plus_idx] = '>';
            if (mul_idx != -1) {
                precedenceTable[plus_idx][mul_idx] = '<';
                precedenceTable[mul_idx][plus_idx] = '>';
            }
            if (lparen_idx != -1) {
                precedenceTable[plus_idx][lparen_idx] = '<';
            }
            if (rparen_idx != -1) {
                precedenceTable[plus_idx][rparen_idx] = '>';
            }
            if (id_idx != -1) {
                precedenceTable[plus_idx][id_idx] = '<';
                precedenceTable[id_idx][plus_idx] = '>';
            }
            if (dollar_idx != -1)
                precedenceTable[plus_idx][dollar_idx] = '>';
        }
        
        // For '*' operator.
        if (mul_idx != -1) {
            precedenceTable[mul_idx][mul_idx] = '>';
            if (lparen_idx != -1) {
                precedenceTable[mul_idx][lparen_idx] = '<';
            }
            if (rparen_idx != -1) {
                precedenceTable[mul_idx][rparen_idx] = '>';
            }
            if (id_idx != -1) {
                precedenceTable[mul_idx][id_idx] = '<';
                precedenceTable[id_idx][mul_idx] = '>';
            }
            if (dollar_idx != -1)
                precedenceTable[mul_idx][dollar_idx] = '>';
        }
        
        // For '('.
        if (lparen_idx != -1) {
            if (plus_idx != -1)
                precedenceTable[lparen_idx][plus_idx] = '<';
            if (mul_idx != -1)
                precedenceTable[lparen_idx][mul_idx] = '<';
            if (id_idx != -1)
                precedenceTable[lparen_idx][id_idx] = '<';
            if (rparen_idx != -1)
                precedenceTable[lparen_idx][rparen_idx] = '=';
        }
        
        // For ')'.
        if (rparen_idx != -1) {
            if (plus_idx != -1)
                precedenceTable[rparen_idx][plus_idx] = '>';
            if (mul_idx != -1)
                precedenceTable[rparen_idx][mul_idx] = '>';
            if (dollar_idx != -1)
                precedenceTable[rparen_idx][dollar_idx] = '>';
        }
        
        // For identifier 'i'.
        if (id_idx != -1) {
            if (plus_idx != -1)
                precedenceTable[id_idx][plus_idx] = '>';
            if (mul_idx != -1)
                precedenceTable[id_idx][mul_idx] = '>';
            if (rparen_idx != -1)
                precedenceTable[id_idx][rparen_idx] = '>';
            if (dollar_idx != -1)
                precedenceTable[id_idx][dollar_idx] = '>';
        }
        
        // For '$'.
        if (dollar_idx != -1) {
            if (id_idx != -1)
                precedenceTable[dollar_idx][id_idx] = '<';
            if (lparen_idx != -1)
                precedenceTable[dollar_idx][lparen_idx] = '<';
            if (plus_idx != -1)
                precedenceTable[dollar_idx][plus_idx] = '<';
            if (mul_idx != -1)
                precedenceTable[dollar_idx][mul_idx] = '<';
            precedenceTable[dollar_idx][dollar_idx] = '=';
        }
    }

    void printPrecedenceTable() {
        printf("\nOperator Precedence Table:\n    ");
        for (int i = 0; i < terminalCount; i++) {
            printf("%c  ", terminals[i]);
        }
        printf("\n");
        
        for (int i = 0; i < terminalCount; i++) {
            printf("%c   ", terminals[i]);
            for (int j = 0; j < terminalCount; j++) {
                printf("%c  ", precedenceTable[i][j]);
            }
            printf("\n");
        }
    }

    // Return the index of the topmost terminal in the stack.
    int findTopmostTerminalIndex() {
        for (int i = top; i >= 0; i--) {
            if (isTerminal(stack[i]))
                return i;
        }
        return -1;
    }

    // Given two terminals, return the precedence relation from the table.
    char getPrecedence(char a, char b) {
        int a_idx = getTerminalIndex(a);
        int b_idx = getTerminalIndex(b);
        if (a_idx != -1 && b_idx != -1)
            return precedenceTable[a_idx][b_idx];
        return ' ';
    }

    // Given indices start and end, extract the substring from the stack into handle,
    // and check if it matches any production’s right-hand side. If so, set *lhs accordingly.
    int matchProduction(int start, int end, char *handle, char *lhs) {
        int len = 0;
        for (int i = start; i <= end; i++) {
            handle[len++] = stack[i];
        }
        handle[len] = '\0';
        
        for (int i = 0; i < productionCount; i++) {
            if (strcmp(grammar[i].production, handle) == 0) {
                *lhs = grammar[i].nonTerminal;
                return i;
            }
        }
        return -1;
    }

    // Find the index of the rightmost terminal in the stack for which the relation
    // between that terminal and its immediate right neighbor is '<'. This marks
    // the handle’s boundary.
    int findHandleBoundaryIndex() {
        int lastTerminal = findTopmostTerminalIndex();
        if (lastTerminal <= 0) return 0; // none found
        int boundary = 0;
        for (int i = 0; i < lastTerminal; i++) {
            if (isTerminal(stack[i]) && isTerminal(stack[i+1])) {
                if (getPrecedence(stack[i], stack[i+1]) == '<')
                    boundary = i;
            }
        }
        return boundary;
    }


    int reduce() {
        int boundary = findHandleBoundaryIndex();
        // There must be at least one symbol after the boundary.
        if (boundary >= top)
            return 0;
        
        for (int start = top; start >= boundary + 1; start--) {
            char handle[MAX_SIZE];
            char lhs;
            int prod_idx = matchProduction(start, top, handle, &lhs);
            if (prod_idx != -1) {
                int handle_len = strlen(handle);
                for (int i = 0; i < handle_len; i++) {
                    pop();
                }
                push(lhs);
                printf("Reduce by %c->%s\n", lhs, handle);
                return 1;
            }
        }
        return 0;
    }

    // Main parsing routine.
    void parse(char *input) {
        char input_buffer[MAX_SIZE];
        strcpy(input_buffer, input);
        strcat(input_buffer, "$");
        
        top = -1;
        push('$');
        
        int input_pos = 0;
        int step = 1;
        
        printf("\nParsing input: %s\n\n", input);
        printf("Step\tStack\t\tInput\t\tAction\n");
        printf("--------------------------------------------------\n");
        
        while (1) {
            // Print current state.
            printf("%d\t", step++);
            for (int k = 0; k <= top; k++) {
                printf("%c", stack[k]);
            }
            printf("\t\t%s\t\t", &input_buffer[input_pos]);
            
            // Acceptance: if stack is "$X" where X is the start symbol (grammar[0].nonTerminal)
            // and input is "$", then accept.
            if (top == 1 && stack[0]=='$' && stack[1]==grammar[0].nonTerminal && input_buffer[input_pos]=='$') {
                printf("Accept\n");
                printf("\nInput string accepted!\n");
                return;
            }
            
            // Find the last terminal in the stack.
            int t_idx = findTopmostTerminalIndex();
            char a = (t_idx != -1) ? stack[t_idx] : '$';
            char b = input_buffer[input_pos];
            char relation = getPrecedence(a, b);
            
            if (relation == '<' || relation == '=') {
                // Shift action.
                push(b);
                printf("Shift %c\n", b);
                input_pos++;
            } else if (relation == '>') {
                // Reduce as long as the relation holds.
                if (!reduce()) {
                    printf("Error: No valid reduction found\n");
                    return;
                } else {
                    printf("Reduce\n");
                }
            } else {
                printf("Error: No precedence relation between %c and %c\n", a, b);
                return;
            }
        }
    }

    int main() {
        printf("Enter the number of productions: ");
        scanf("%d", &productionCount);
        getchar();  // Consume newline
        
        printf("Enter the productions in the format A->α (e.g., E->E+T):\n");
        for (int i = 0; i < productionCount; i++) {
            char inputProd[MAX_SIZE];
            printf("Production %d: ", i + 1);
            fgets(inputProd, MAX_SIZE, stdin);
            
            // Remove trailing newline.
            size_t len = strlen(inputProd);
            if (len > 0 && inputProd[len-1] == '\n') {
                inputProd[len-1] = '\0';
                len--;
            }
            
            if (len < 4 || inputProd[1] != '-' || inputProd[2] != '>') {
                printf("Invalid format! Use A->α format (e.g., E->E+T). Try again.\n");
                i--;
                continue;
            }
            grammar[i].nonTerminal = inputProd[0];
            strcpy(grammar[i].production, inputProd + 3);
        }
        
        identifySymbols();
        
        printf("\nTerminals identified: { ");
        for (int i = 0; i < terminalCount; i++)
            printf("%c ", terminals[i]);
        printf("}\n");
        
        printf("\nNon-Terminals identified: { ");
        for (int i = 0; i < nonTerminalCount; i++)
            printf("%c ", nonTerminals[i]);
        printf("}\n");
        
        setupPrecedenceTable();
        printPrecedenceTable();
        
        char inputStr[MAX_SIZE];
        printf("\nEnter the input string to parse (use 'i' for identifiers): ");
        scanf("%s", inputStr);
        
        parse(inputStr);
        
        return 0;
    }
