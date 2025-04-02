#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTIONS 50
#define MAX_SYMBOLS 100
#define MAX_STATES 100
#define MAX_STACK 100
#define MAX_STRING 100

typedef struct {
    char lhs;
    char rhs[20];
    int length;
} Production;

typedef struct {
    char lhs;
    char rhs[20];
    int dot_pos;
    int prod_num;
} Item;

typedef struct {
    Item items[MAX_PRODUCTIONS * 2];
    int num_items;
} State;

typedef struct {
    State states[MAX_STATES];
    int transitions[MAX_STATES][MAX_SYMBOLS];
    char symbols[MAX_SYMBOLS];
    int num_states;
    int num_symbols;
} CanonicalCollection;

typedef struct {
    int action[MAX_STATES][MAX_SYMBOLS];  // Positive: shift, Negative: reduce, 0: error, -1000: accept
    int goto_table[MAX_STATES][MAX_SYMBOLS];
    char terminals[MAX_SYMBOLS];
    char non_terminals[MAX_SYMBOLS];
    int num_terminals;
    int num_non_terminals;
} ParsingTable;

Production productions[MAX_PRODUCTIONS];
int num_productions = 0;
CanonicalCollection collection;
ParsingTable parsing_table;
char start_symbol;

// Helper functions
int is_non_terminal(char c) {
    return isupper(c);
}

int is_terminal(char c) {
    return !is_non_terminal(c) && c != '.' && c != '\0' && c != '@';  // @ represents epsilon
}

int symbol_index(char symbol, char *arr, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == symbol) {
            return i;
        }
    }
    return -1;
}

// Compute FIRST set for grammar symbols
void compute_first_sets(char first[MAX_SYMBOLS][MAX_SYMBOLS], int *first_sizes) {
    // Initialize FIRST sets for all symbols
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        first_sizes[i] = 0;
    }
    
    // For all terminals, FIRST contains only the terminal itself
    for (int i = 0; i < parsing_table.num_terminals; i++) {
        char terminal = parsing_table.terminals[i];
        if (terminal != '$') {  // Skip $ as it's not really a terminal
            first[terminal][0] = terminal;
            first_sizes[terminal] = 1;
        }
    }
    
    // Compute FIRST for non-terminals
    int changed = 1;
    while (changed) {
        changed = 0;
        
        for (int i = 0; i < num_productions; i++) {
            char lhs = productions[i].lhs;
            char *rhs = productions[i].rhs;
            
            // If production is A -> ε, add ε to FIRST(A)
            if (rhs[0] == '@') {
                if (symbol_index('@', first[lhs], first_sizes[lhs]) == -1) {
                    first[lhs][first_sizes[lhs]++] = '@';
                    changed = 1;
                }
                continue;
            }
            
            // For each symbol in the RHS
            int j = 0;
            int can_derive_epsilon = 1;
            
            while (rhs[j] != '\0' && can_derive_epsilon) {
                char current = rhs[j];
                can_derive_epsilon = 0;
                
                // If current symbol is terminal, add it to FIRST(lhs)
                if (is_terminal(current)) {
                    if (symbol_index(current, first[lhs], first_sizes[lhs]) == -1) {
                        first[lhs][first_sizes[lhs]++] = current;
                        changed = 1;
                    }
                    break;
                } 
                // If current symbol is non-terminal
                else if (is_non_terminal(current)) {
                    // Add all elements in FIRST(current) except ε to FIRST(lhs)
                    for (int k = 0; k < first_sizes[current]; k++) {
                        if (first[current][k] != '@') {
                            if (symbol_index(first[current][k], first[lhs], first_sizes[lhs]) == -1) {
                                first[lhs][first_sizes[lhs]++] = first[current][k];
                                changed = 1;
                            }
                        }
                    }
                    
                    // Check if current can derive epsilon
                    if (symbol_index('@', first[current], first_sizes[current]) != -1) {
                        can_derive_epsilon = 1;
                    }
                }
                
                j++;
            }
            
            // If all symbols in RHS can derive epsilon, add epsilon to FIRST(lhs)
            if (can_derive_epsilon && rhs[j] == '\0') {
                if (symbol_index('@', first[lhs], first_sizes[lhs]) == -1) {
                    first[lhs][first_sizes[lhs]++] = '@';
                    changed = 1;
                }
            }
        }
    }
}

// Compute FOLLOW set for non-terminals
void compute_follow_sets(char follow[MAX_SYMBOLS][MAX_SYMBOLS], int *follow_sizes, char first[MAX_SYMBOLS][MAX_SYMBOLS], int *first_sizes) {
    // Initialize FOLLOW sets for all non-terminals
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        follow_sizes[i] = 0;
    }
    
    // Add $ to FOLLOW(start_symbol)
    follow[start_symbol][0] = '$';
    follow_sizes[start_symbol] = 1;
    
    // Repeatedly apply FOLLOW set rules until no more changes
    int changed = 1;
    while (changed) {
        changed = 0;
        
        for (int i = 0; i < num_productions; i++) {
            char lhs = productions[i].lhs;
            char *rhs = productions[i].rhs;
            int rhs_len = strlen(rhs);
            
            // Skip epsilon productions
            if (rhs[0] == '@') continue;
            
            // For each symbol in the RHS
            for (int j = 0; j < rhs_len; j++) {
                char current = rhs[j];
                
                // If current is a non-terminal
                if (is_non_terminal(current)) {
                    int epsilon_in_remaining = 1;
                    
                    // Process symbols after current in the RHS
                    if (j < rhs_len - 1) {
                        int k = j + 1;
                        while (k < rhs_len && epsilon_in_remaining) {
                            char next = rhs[k];
                            
                            // If next is terminal, add it to FOLLOW(current)
                            if (is_terminal(next)) {
                                if (symbol_index(next, follow[current], follow_sizes[current]) == -1) {
                                    follow[current][follow_sizes[current]++] = next;
                                    changed = 1;
                                }
                                epsilon_in_remaining = 0;
                            } 
                            // If next is non-terminal
                            else if (is_non_terminal(next)) {
                                // Add all elements in FIRST(next) except ε to FOLLOW(current)
                                for (int m = 0; m < first_sizes[next]; m++) {
                                    if (first[next][m] != '@') {
                                        if (symbol_index(first[next][m], follow[current], follow_sizes[current]) == -1) {
                                            follow[current][follow_sizes[current]++] = first[next][m];
                                            changed = 1;
                                        }
                                    }
                                }
                                
                                // Check if next can derive epsilon
                                if (symbol_index('@', first[next], first_sizes[next]) == -1) {
                                    epsilon_in_remaining = 0;
                                }
                            }
                            
                            k++;
                        }
                    }
                    
                    // If current is the last symbol or all symbols after current can derive epsilon
                    // Add all elements in FOLLOW(lhs) to FOLLOW(current)
                    if (j == rhs_len - 1 || epsilon_in_remaining) {
                        for (int k = 0; k < follow_sizes[lhs]; k++) {
                            if (symbol_index(follow[lhs][k], follow[current], follow_sizes[current]) == -1) {
                                follow[current][follow_sizes[current]++] = follow[lhs][k];
                                changed = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}

// Function to add a new item to a state
void add_item(State *state, char lhs, char *rhs, int dot_pos, int prod_num) {
    for (int i = 0; i < state->num_items; i++) {
        if (state->items[i].lhs == lhs && 
            state->items[i].dot_pos == dot_pos && 
            strcmp(state->items[i].rhs, rhs) == 0) {
            return;  // Item already exists
        }
    }
    
    Item new_item;
    new_item.lhs = lhs;
    strcpy(new_item.rhs, rhs);
    new_item.dot_pos = dot_pos;
    new_item.prod_num = prod_num;
    
    state->items[state->num_items++] = new_item;
}

// Compute the closure of a state
void compute_closure(State *state) {
    int changed = 1;
    
    while (changed) {
        changed = 0;
        
        for (int i = 0; i < state->num_items; i++) {
            // If dot is before a non-terminal
            if (state->items[i].dot_pos < strlen(state->items[i].rhs) && 
                is_non_terminal(state->items[i].rhs[state->items[i].dot_pos])) {
                
                char non_terminal = state->items[i].rhs[state->items[i].dot_pos];
                
                // Find all productions for this non-terminal
                for (int j = 0; j < num_productions; j++) {
                    if (productions[j].lhs == non_terminal) {
                        // Add new item with dot at position 0
                        if (strcmp(productions[j].rhs, "@") == 0) {
                            // Handle epsilon production
                            add_item(state, non_terminal, "", 0, j);
                        } else {
                            add_item(state, non_terminal, productions[j].rhs, 0, j);
                        }
                        changed = 1;
                    }
                }
            }
        }
    }
}

// Create a new state by shifting the dot for a given symbol
void goto_state(State *current, char symbol, State *result) {
    result->num_items = 0;
    
    for (int i = 0; i < current->num_items; i++) {
        Item item = current->items[i];
        
        // If dot is not at the end and next symbol matches
        if (item.dot_pos < strlen(item.rhs) && item.rhs[item.dot_pos] == symbol) {
            // Add item with dot shifted right
            add_item(result, item.lhs, item.rhs, item.dot_pos + 1, item.prod_num);
        }
    }
    
    // Compute closure of the new state
    compute_closure(result);
}

// Check if two states are equal
int states_equal(State *state1, State *state2) {
    if (state1->num_items != state2->num_items) {
        return 0;
    }
    
    for (int i = 0; i < state1->num_items; i++) {
        int found = 0;
        
        for (int j = 0; j < state2->num_items; j++) {
            if (state1->items[i].lhs == state2->items[j].lhs && 
                state1->items[i].dot_pos == state2->items[j].dot_pos && 
                strcmp(state1->items[i].rhs, state2->items[j].rhs) == 0) {
                found = 1;
                break;
            }
        }
        
        if (!found) {
            return 0;
        }
    }
    
    return 1;
}

// Find symbols that can follow the dot in a state
void find_symbols_after_dot(State *state, char *symbols, int *num_symbols) {
    *num_symbols = 0;
    
    for (int i = 0; i < state->num_items; i++) {
        Item item = state->items[i];
        
        // If dot is not at the end
        if (item.dot_pos < strlen(item.rhs)) {
            char symbol = item.rhs[item.dot_pos];
            int found = 0;
            
            // Check if symbol is already in the list
            for (int j = 0; j < *num_symbols; j++) {
                if (symbols[j] == symbol) {
                    found = 1;
                    break;
                }
            }
            
            // Add symbol if not already in the list
            if (!found) {
                symbols[(*num_symbols)++] = symbol;
            }
        }
    }
}

// Generate the canonical collection of LR(0) items
void generate_canonical_collection() {
    // Initialize collection
    collection.num_states = 0;
    collection.num_symbols = 0;
    
    // Create initial state with [Z -> .<start_symbol>]
    State initial_state;
    initial_state.num_items = 0;
    char start_str[2] = {start_symbol, '\0'};
    add_item(&initial_state, 'Z', start_str, 0, -1);  // Special production Z -> <start_symbol>
    
    // Compute closure of initial state
    compute_closure(&initial_state);
    
    // Add initial state to collection
    collection.states[0] = initial_state;
    collection.num_states = 1;
    
    // Initialize transitions to -1
    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < MAX_SYMBOLS; j++) {
            collection.transitions[i][j] = -1;
        }
    }
    
    // Generate all states
    for (int i = 0; i < collection.num_states; i++) {
        char symbols[MAX_SYMBOLS];
        int num_symbols;
        
        // Find all symbols that can follow the dot in this state
        find_symbols_after_dot(&collection.states[i], symbols, &num_symbols);
        
        // For each symbol, create a new state
        for (int j = 0; j < num_symbols; j++) {
            State new_state;
            goto_state(&collection.states[i], symbols[j], &new_state);
            
            // Check if the new state already exists
            int found = -1;
            for (int k = 0; k < collection.num_states; k++) {
                if (states_equal(&new_state, &collection.states[k])) {
                    found = k;
                    break;
                }
            }
            
            // Add new state if it doesn't exist
            if (found == -1) {
                collection.states[collection.num_states] = new_state;
                found = collection.num_states++;
            }
            
            // Record transition
            collection.transitions[i][(int)symbols[j]] = found;
            
            // Add symbol to the list of symbols if it's not already there
            int symbol_found = 0;
            for (int k = 0; k < collection.num_symbols; k++) {
                if (collection.symbols[k] == symbols[j]) {
                    symbol_found = 1;
                    break;
                }
            }
            
            if (!symbol_found) {
                collection.symbols[collection.num_symbols++] = symbols[j];
            }
        }
    }
}

// Create the SLR parsing table
void create_parsing_table(char follow[MAX_SYMBOLS][MAX_SYMBOLS], int *follow_sizes) {
    // Initialize parsing table
    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < MAX_SYMBOLS; j++) {
            parsing_table.action[i][j] = 0;    // Error
            parsing_table.goto_table[i][j] = -1;  // No goto
        }
    }
    
    // Identify terminals and non-terminals
    parsing_table.num_terminals = 0;
    parsing_table.num_non_terminals = 0;
    
    for (int i = 0; i < collection.num_symbols; i++) {
        char symbol = collection.symbols[i];
        
        if (is_terminal(symbol)) {
            parsing_table.terminals[parsing_table.num_terminals++] = symbol;
        } else if (is_non_terminal(symbol)) {
            parsing_table.non_terminals[parsing_table.num_non_terminals++] = symbol;
        }
    }
    
    // Add $ to terminals if not already there
    if (symbol_index('$', parsing_table.terminals, parsing_table.num_terminals) == -1) {
        parsing_table.terminals[parsing_table.num_terminals++] = '$';
    }
    
    // Process each state to fill the parsing table
    for (int i = 0; i < collection.num_states; i++) {
        State state = collection.states[i];
        
        // Process each item in the state
        for (int j = 0; j < state.num_items; j++) {
            Item item = state.items[j];
            
            // Case 1: [A -> α.aβ] => action[i, a] = shift to state k
            if (item.dot_pos < strlen(item.rhs)) {
                char symbol = item.rhs[item.dot_pos];
                
                if (is_terminal(symbol)) {
                    int next_state = collection.transitions[i][(int)symbol];
                    
                    // If next_state >= 0, add shift action
                    if (next_state >= 0) {
                        int symbol_idx = symbol_index(symbol, parsing_table.terminals, parsing_table.num_terminals);
                        
                        // Check for conflicts
                        if (parsing_table.action[i][symbol_idx] != 0 && 
                            parsing_table.action[i][symbol_idx] != next_state + 1) {
                            printf("Shift-Reduce or Shift-Shift conflict at state %d, symbol %c\n", i, symbol);
                        }
                        
                        parsing_table.action[i][symbol_idx] = next_state + 1;  // Shift to state
                    }
                }
            }
            // Case 2: [A -> α.] => action[i, a] = reduce by A -> α for all a in FOLLOW(A)
            else if (item.prod_num >= 0) {  // Skip the augmented production
                for (int k = 0; k < follow_sizes[item.lhs]; k++) {
                    char follow_symbol = follow[item.lhs][k];
                    int symbol_idx = symbol_index(follow_symbol, parsing_table.terminals, parsing_table.num_terminals);
                    
                    if (symbol_idx >= 0) {
                        // Check for conflicts
                        if (parsing_table.action[i][symbol_idx] != 0 && 
                            parsing_table.action[i][symbol_idx] != -(item.prod_num + 1)) {
                            printf("Reduce-Reduce or Shift-Reduce conflict at state %d, symbol %c\n", i, follow_symbol);
                        }
                        
                        parsing_table.action[i][symbol_idx] = -(item.prod_num + 1);  // Reduce by production
                    }
                }
            }
            // Case 3: [Z -> <start_symbol>.] => action[i, $] = accept
            else if (item.lhs == 'Z' && item.dot_pos == 1 && item.rhs[0] == start_symbol) {
                int symbol_idx = symbol_index('$', parsing_table.terminals, parsing_table.num_terminals);
                parsing_table.action[i][symbol_idx] = -1000;  // Accept
            }
        }
        
        // Fill GOTO table
        for (int j = 0; j < parsing_table.num_non_terminals; j++) {
            char symbol = parsing_table.non_terminals[j];
            int next_state = collection.transitions[i][(int)symbol];
            
            if (next_state >= 0) {
                parsing_table.goto_table[i][j] = next_state;
            }
        }
    }
}

// Parse an input string using the SLR parsing table
int parse_input(char *input) {
    int stack[MAX_STACK];
    int top = 0;
    stack[top] = 0;  // Initial state
    
    int input_pos = 0;
    char current_symbol = input[input_pos];
    
    printf("\nParsing Steps:\n");
    printf("%-20s %-20s %-20s %-20s\n", "Stack", "Input", "Action", "Production");
    
    while (1) {
        // Print current stack and input
        char stack_str[MAX_STRING] = "";
        for (int i = 0; i <= top; i++) {
            char temp[10];
            sprintf(temp, "%d ", stack[i]);
            strcat(stack_str, temp);
        }
        
        char input_str[MAX_STRING] = "";
        for (int i = input_pos; input[i] != '\0'; i++) {
            char temp[2] = {input[i], '\0'};
            strcat(input_str, temp);
        }
        
        printf("%-20s %-20s ", stack_str, input_str);
        
        // Get current state
        int state = stack[top];
        
        // Find index of current symbol in terminals
        int symbol_idx = symbol_index(current_symbol, parsing_table.terminals, parsing_table.num_terminals);
        
        if (symbol_idx == -1) {
            printf("Error: Symbol %c not found in terminals\n", current_symbol);
            return 0;  // Failure
        }
        
        // Get action
        int action = parsing_table.action[state][symbol_idx];
        
        // Perform action
        if (action > 0) {  // Shift
            int next_state = action - 1;
            printf("%-20s\n", "Shift");
            
            // Push next state
            top++;
            stack[top] = next_state;
            
            // Move to next input symbol
            input_pos++;
            current_symbol = input[input_pos];
        } 
        else if (action < 0 && action != -1000) {  // Reduce
            int prod_num = -action - 1;
            
            printf("%-20s ", "Reduce");
            printf("%c -> %s\n", productions[prod_num].lhs, productions[prod_num].rhs);
            
            // Pop |RHS| elements from stack (each production symbol corresponds to one state)
            int rhs_len = strlen(productions[prod_num].rhs);
            if (strcmp(productions[prod_num].rhs, "@") == 0) {
                rhs_len = 0;  // Handle epsilon production
            }
            
            top -= rhs_len;
            
            // Get state at the top of stack
            int current_state = stack[top];
            
            // Find index of LHS in non-terminals
            int lhs_idx = symbol_index(productions[prod_num].lhs, parsing_table.non_terminals, parsing_table.num_non_terminals);
            
            // Get goto state
            int goto_state = parsing_table.goto_table[current_state][lhs_idx];
            
            // Push goto state
            top++;
            stack[top] = goto_state;
        } 
        else if (action == -1000) {  // Accept
            printf("%-20s\n", "Accept");
            return 1;  // Success
        } 
        else {  // Error
            printf("%-20s\n", "Error");
            return 0;  // Failure
        }
    }
}

int main() {
    printf("SLR Parser\n");
    printf("==========\n\n");
    
    // Get grammar
    printf("Enter the number of productions: ");
    scanf("%d", &num_productions);
    getchar();  // Consume newline
    
    printf("Enter productions in the form A->α (use @ for epsilon):\n");
    for (int i = 0; i < num_productions; i++) {
        char input[50];
        printf("Production %d: ", i + 1);
        fgets(input, sizeof(input), stdin);
        
        // Remove newline character
        input[strcspn(input, "\n")] = '\0';
        
        // Parse production
        if (strlen(input) >= 3 && input[1] == '-' && input[2] == '>') {
            productions[i].lhs = input[0];
            strcpy(productions[i].rhs, &input[3]);
            productions[i].length = strlen(productions[i].rhs);
        } else {
            printf("Invalid production format. Use A->α\n");
            i--;
        }
    }
    
    // Set start symbol
    start_symbol = productions[0].lhs;
    
    // Display grammar
    printf("\nGrammar:\n");
    for (int i = 0; i < num_productions; i++) {
        printf("%d. %c -> %s\n", i + 1, productions[i].lhs, productions[i].rhs);
    }
    
    // Generate canonical collection
    generate_canonical_collection();
    
    // Display canonical collection
    printf("\nCanonical Collection of LR(0) Items:\n");
    for (int i = 0; i < collection.num_states; i++) {
        printf("State %d:\n", i);
        for (int j = 0; j < collection.states[i].num_items; j++) {
            Item item = collection.states[i].items[j];
            printf("  %c -> ", item.lhs);
            
            // Print RHS with dot
            for (int k = 0; k < strlen(item.rhs); k++) {
                if (k == item.dot_pos) {
                    printf(".");
                }
                printf("%c", item.rhs[k]);
            }
            
            // If dot is at the end
            if (item.dot_pos == strlen(item.rhs)) {
                printf(".");
            }
            
            printf("\n");
        }
    }
    
    // Display transitions
    printf("\nTransitions:\n");
    for (int i = 0; i < collection.num_states; i++) {
        for (int j = 0; j < collection.num_symbols; j++) {
            char symbol = collection.symbols[j];
            int next_state = collection.transitions[i][(int)symbol];
            
            if (next_state >= 0) {
                printf("GOTO(%d, %c) = %d\n", i, symbol, next_state);
            }
        }
    }
    
    // Compute FIRST and FOLLOW sets
    char first[MAX_SYMBOLS][MAX_SYMBOLS];
    int first_sizes[MAX_SYMBOLS];
    char follow[MAX_SYMBOLS][MAX_SYMBOLS];
    int follow_sizes[MAX_SYMBOLS];
    
    compute_first_sets(first, first_sizes);
    compute_follow_sets(follow, follow_sizes, first, first_sizes);
    
    // Display FIRST sets
    printf("\nFIRST sets:\n");
    for (int i = 0; i < parsing_table.num_non_terminals; i++) {
        char symbol = parsing_table.non_terminals[i];
        printf("FIRST(%c) = { ", symbol);
        for (int j = 0; j < first_sizes[symbol]; j++) {
            printf("%c ", first[symbol][j]);
        }
        printf("}\n");
    }
    
    // Display FOLLOW sets
    printf("\nFOLLOW sets:\n");
    for (int i = 0; i < parsing_table.num_non_terminals; i++) {
        char symbol = parsing_table.non_terminals[i];
        printf("FOLLOW(%c) = { ", symbol);
        for (int j = 0; j < follow_sizes[symbol]; j++) {
            printf("%c ", follow[symbol][j]);
        }
        printf("}\n");
    }
    
    // Create parsing table
    create_parsing_table(follow, follow_sizes);
    
    // Display parsing table
    printf("\nParsing Table:\n");
    printf("%-10s", "STATE");
    
    // Print terminal symbols (ACTION table)
    for (int i = 0; i < parsing_table.num_terminals; i++) {
        printf("%-10c", parsing_table.terminals[i]);
    }
    
    // Print non-terminal symbols (GOTO table)
    for (int i = 0; i < parsing_table.num_non_terminals; i++) {
        printf("%-10c", parsing_table.non_terminals[i]);
    }
    printf("\n");
    
    // Print table rows
    for (int i = 0; i < collection.num_states; i++) {
        printf("%-10d", i);
        
        // Print ACTION entries
        for (int j = 0; j < parsing_table.num_terminals; j++) {
            int action = parsing_table.action[i][j];
            
            if (action > 0) {
                printf("s%-9d", action - 1);
            } else if (action < 0 && action != -1000) {
                printf("r%-9d", -action - 1);
            } else if (action == -1000) {
                printf("%-10s", "ACC");
            } else {
                printf("%-10s", "");
            }
        }
        
        // Print GOTO entries
        for (int j = 0; j < parsing_table.num_non_terminals; j++) {
            int goto_state = parsing_table.goto_table[i][j];
            
            if (goto_state >= 0) {
                printf("%-10d", goto_state);
            } else {
                printf("%-10s", "");
            }
        }
        
        printf("\n");
    }
    
    // Parse input string
    char input[MAX_STRING];
    printf("\nEnter a string to parse (end with $): ");
    scanf("%s", input);
    
    if (parse_input(input)) {
        printf("\nString accepted!\n");
    } else {
        printf("\nString rejected!\n");
    }
    
    return 0;
}
