#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTIONS 15
#define MAX_STATES 50
#define MAX_SYMBOLS 20
#define MAX_TERMINALS 20
#define MAX_NON_TERMINALS 10
#define MAX_STRING 100

// Structure to represent an item in a state
typedef struct {
    char lhs;
    char rhs[MAX_SYMBOLS];
    char lookahead;
} Item;

// Structure to represent a state in the DFA
typedef struct {
    Item items[MAX_PRODUCTIONS * 2];
    int num_items;
} State;

// Action table entry
typedef struct {
    char action; // 's' for shift, 'r' for reduce, 'a' for accept, ' ' for error
    int value;   // state number for shift, production number for reduce
} ActionEntry;

// Global variables
State states[MAX_STATES];
int num_states = 0;
char goto_table[MAX_STATES][MAX_SYMBOLS];
int goto_destinations[MAX_STATES][MAX_SYMBOLS];
char productions_lhs[MAX_PRODUCTIONS];
char productions_rhs[MAX_PRODUCTIONS][MAX_SYMBOLS];
int num_productions = 0;
char terminals[MAX_TERMINALS];
int num_terminals = 0;
char non_terminals[MAX_NON_TERMINALS];
int num_non_terminals = 0;
ActionEntry action_table[MAX_STATES][MAX_TERMINALS + 1]; // +1 for '$'

// Function to check if a character is a terminal
int is_terminal(char c) {
    if (isupper(c) || c == '$' || c == '#')
        return 0;
    return 1;
}

// Function to check if a character is a non-terminal
int is_non_terminal(char c) {
    if (isupper(c))
        return 1;
    return 0;
}

// Function to add a terminal to the list if it's not already there
void add_terminal(char t) {
    for (int i = 0; i < num_terminals; i++) {
        if (terminals[i] == t)
            return;
    }
    terminals[num_terminals++] = t;
}

// Function to add a non-terminal to the list if it's not already there
void add_non_terminal(char nt) {
    for (int i = 0; i < num_non_terminals; i++) {
        if (non_terminals[i] == nt)
            return;
    }
    non_terminals[num_non_terminals++] = nt;
}

// Function to check if an item already exists in a state
int item_exists(State *state, Item item) {
    for (int i = 0; i < state->num_items; i++) {
        if (state->items[i].lhs == item.lhs && 
            strcmp(state->items[i].rhs, item.rhs) == 0 &&
            state->items[i].lookahead == item.lookahead) {
            return 1;
        }
    }
    return 0;
}

// Function to add an item to a state if it doesn't already exist
void add_item(State *state, Item item) {
    if (!item_exists(state, item)) {
        state->items[state->num_items++] = item;
    }
}

// Function to get the position of the dot in an item
int get_dot_position(char *rhs) {
    char *dot = strchr(rhs, '.');
    if (dot)
        return dot - rhs;
    return -1;
}

// Function to compute the closure of a state
void compute_closure(State *state) {
    int added = 1;
    
    while (added) {
        added = 0;
        
        for (int i = 0; i < state->num_items; i++) {
            int dot_pos = get_dot_position(state->items[i].rhs);
            
            if (dot_pos != -1 && state->items[i].rhs[dot_pos + 1] != '\0') {
                char next_symbol = state->items[i].rhs[dot_pos + 1];
                
                if (is_non_terminal(next_symbol)) {
                    for (int j = 0; j < num_productions; j++) {
                        if (productions_lhs[j] == next_symbol) {
                            Item new_item;
                            new_item.lhs = productions_lhs[j];
                            strcpy(new_item.rhs, productions_rhs[j]);
                            
                            // Add dot at the beginning
                            memmove(new_item.rhs + 1, new_item.rhs, strlen(new_item.rhs) + 1);
                            new_item.rhs[0] = '.';
                            
                            // Set lookahead
                            // In a simple implementation, we'll use the lookahead from the original item
                            // In a full LALR parser, this would be computed from FOLLOW sets
                            new_item.lookahead = state->items[i].lookahead;
                            
                            if (!item_exists(state, new_item)) {
                                state->items[state->num_items++] = new_item;
                                added = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}

// Function to compare two states for equality
int states_equal(State *s1, State *s2) {
    if (s1->num_items != s2->num_items)
        return 0;
    
    for (int i = 0; i < s1->num_items; i++) {
        if (!item_exists(s2, s1->items[i]))
            return 0;
    }
    
    return 1;
}

// Function to compute the GOTO of a state on a symbol
int compute_goto(State *source, char symbol, State *result) {
    result->num_items = 0;
    
    for (int i = 0; i < source->num_items; i++) {
        int dot_pos = get_dot_position(source->items[i].rhs);
        
        if (dot_pos != -1 && source->items[i].rhs[dot_pos + 1] == symbol) {
            Item new_item;
            new_item.lhs = source->items[i].lhs;
            strcpy(new_item.rhs, source->items[i].rhs);
            new_item.lookahead = source->items[i].lookahead;
            
            // Move the dot past the symbol
            new_item.rhs[dot_pos] = symbol;
            new_item.rhs[dot_pos + 1] = '.';
            
            add_item(result, new_item);
        }
    }
    
    if (result->num_items > 0) {
        compute_closure(result);
        return 1;
    }
    
    return 0;
}

// Function to find or create a state
int find_or_create_state(State *new_state) {
    for (int i = 0; i < num_states; i++) {
        if (states_equal(&states[i], new_state)) {
            return i;
        }
    }
    
    states[num_states] = *new_state;
    return num_states++;
}

// Function to construct the LALR parsing table
void construct_lalr_table() {
    // Initialize action table with errors
    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < MAX_TERMINALS + 1; j++) {
            action_table[i][j].action = ' ';
            action_table[i][j].value = -1;
        }
    }
    
    // Create the initial state
    State initial_state;
    initial_state.num_items = 1;
    initial_state.items[0].lhs = 'Z';  // Augmented grammar starts with Z
    sprintf(initial_state.items[0].rhs, ".%c", productions_lhs[0]);  // Z -> .S
    initial_state.items[0].lookahead = '$';  // End marker
    
    compute_closure(&initial_state);
    states[0] = initial_state;
    num_states = 1;
    
    // Construct the DFA
    for (int i = 0; i < num_states; i++) {
        // For each state and each grammar symbol
        for (int t = 0; t < num_terminals; t++) {
            State new_state;
            if (compute_goto(&states[i], terminals[t], &new_state)) {
                int goto_state = find_or_create_state(&new_state);
                goto_table[i][terminals[t]] = terminals[t];
                goto_destinations[i][terminals[t]] = goto_state;
                
                // Add shift action
                action_table[i][t].action = 's';
                action_table[i][t].value = goto_state;
            }
        }
        
        for (int nt = 0; nt < num_non_terminals; nt++) {
            State new_state;
            if (compute_goto(&states[i], non_terminals[nt], &new_state)) {
                int goto_state = find_or_create_state(&new_state);
                goto_table[i][non_terminals[nt]] = non_terminals[nt];
                goto_destinations[i][non_terminals[nt]] = goto_state;
            }
        }
        
        // Look for reduction actions
        for (int j = 0; j < states[i].num_items; j++) {
            int dot_pos = get_dot_position(states[i].items[j].rhs);
            
            if (states[i].items[j].rhs[dot_pos + 1] == '\0') {  // Dot at the end
                char lookahead = states[i].items[j].lookahead;
                int term_index = -1;
                
                for (int t = 0; t < num_terminals; t++) {
                    if (terminals[t] == lookahead) {
                        term_index = t;
                        break;
                    }
                }
                
                if (term_index == -1 && lookahead == '$') {
                    term_index = num_terminals;  // $ is at the end
                }
                
                if (term_index != -1) {
                    // Find the production number
                    int prod_num = -1;
                    char lhs = states[i].items[j].lhs;
                    char rhs[MAX_SYMBOLS];
                    strcpy(rhs, states[i].items[j].rhs);
                    rhs[dot_pos] = '\0';  // Remove dot for comparison
                    
                    for (int p = 0; p < num_productions; p++) {
                        if (productions_lhs[p] == lhs && strcmp(productions_rhs[p], rhs) == 0) {
                            prod_num = p;
                            break;
                        }
                    }
                    
                    if (prod_num != -1) {
                        // Check for accept action (Z -> S.)
                        if (lhs == 'Z' && lookahead == '$') {
                            action_table[i][term_index].action = 'a';
                            action_table[i][term_index].value = 0;
                        } else {
                            // Add reduce action
                            action_table[i][term_index].action = 'r';
                            action_table[i][term_index].value = prod_num;
                        }
                    }
                }
            }
        }
    }
}

// Function to print the LALR parsing table
void print_lalr_table() {
    printf("\nLALR(1) PARSING TABLE:\n\n");
    
    // Print header with terminals
    printf("STATE | ");
    for (int t = 0; t < num_terminals; t++) {
        printf("%c | ", terminals[t]);
    }
    printf("$ | ");
    
    // Print non-terminals
    for (int nt = 0; nt < num_non_terminals; nt++) {
        printf("%c | ", non_terminals[nt]);
    }
    printf("\n");
    
    // Print separator
    for (int i = 0; i < 7 + (num_terminals + 1) * 4 + num_non_terminals * 4; i++) {
        printf("-");
    }
    printf("\n");
    
    // Print table rows
    for (int s = 0; s < num_states; s++) {
        printf("%-5d | ", s);
        
        // Print action part
        for (int t = 0; t < num_terminals; t++) {
            if (action_table[s][t].action != ' ') {
                printf("%c%d | ", action_table[s][t].action, action_table[s][t].value);
            } else {
                printf("   | ");
            }
        }
        
        // Print action for $
        if (action_table[s][num_terminals].action != ' ') {
            printf("%c%d | ", action_table[s][num_terminals].action, action_table[s][num_terminals].value);
        } else {
            printf("   | ");
        }
        
        // Print goto part
        for (int nt = 0; nt < num_non_terminals; nt++) {
            char symbol = non_terminals[nt];
            if (goto_table[s][symbol] == symbol) {
                printf("%-2d | ", goto_destinations[s][symbol]);
            } else {
                printf("   | ");
            }
        }
        
        printf("\n");
    }
    
    // Print productions
    printf("\nPRODUCTIONS:\n");
    for (int i = 0; i < num_productions; i++) {
        printf("%d: %c -> %s\n", i, productions_lhs[i], productions_rhs[i]);
    }
}

// Function to print the DFA states
void print_states() {
    printf("\nLALR(1) DFA STATES:\n");
    for (int i = 0; i < num_states; i++) {
        printf("\nI%d:\n", i);
        for (int j = 0; j < states[i].num_items; j++) {
            printf("  %c -> %s, %c\n", states[i].items[j].lhs, states[i].items[j].rhs, states[i].items[j].lookahead);
        }
        
        printf("  Transitions: ");
        for (int s = 0; s < MAX_SYMBOLS; s++) {
            if (goto_table[i][s] != 0) {
                printf("%c->I%d ", goto_table[i][s], goto_destinations[i][s]);
            }
        }
        printf("\n");
    }
}

// Function to parse an input string using the LALR parsing table
int parse_input(char *input) {
    printf("\nParsing input: %s\n", input);
    
    // Add $ to the end of input
    strcat(input, "$");
    
    // Initialize stack with state 0
    int stack[100];
    int top = 0;
    stack[0] = 0;
    
    int input_pos = 0;
    char current_symbol = input[input_pos];
    
    printf("\nParsing Steps:\n");
    printf("%-20s %-20s %-20s %-20s\n", "Stack", "Input", "Action", "Production");
    
    while (1) {
        int current_state = stack[top];
        
        // Find the index of the current symbol in terminals
        int term_index = -1;
        for (int t = 0; t < num_terminals; t++) {
            if (terminals[t] == current_symbol) {
                term_index = t;
                break;
            }
        }
        
        if (term_index == -1 && current_symbol == '$') {
            term_index = num_terminals;  // $ is at the end
        }
        
        if (term_index == -1) {
            printf("Error: Symbol %c not recognized\n", current_symbol);
            return 0;
        }
        
        // Get the action
        char action = action_table[current_state][term_index].action;
        int value = action_table[current_state][term_index].value;
        
        // Print parsing step
        char stack_str[100] = "";
        for (int i = 0; i <= top; i++) {
            char temp[10];
            sprintf(temp, "%d ", stack[i]);
            strcat(stack_str, temp);
        }
        
        char input_str[100] = "";
        strcat(input_str, input + input_pos);
        
        char action_str[100] = "";
        if (action == 's') {
            sprintf(action_str, "Shift %d", value);
        } else if (action == 'r') {
            sprintf(action_str, "Reduce by %d", value);
        } else if (action == 'a') {
            strcpy(action_str, "Accept");
        } else {
            strcpy(action_str, "Error");
        }
        
        char production_str[100] = "";
        if (action == 'r') {
            sprintf(production_str, "%c -> %s", productions_lhs[value], productions_rhs[value]);
        }
        
        printf("%-20s %-20s %-20s %-20s\n", stack_str, input_str, action_str, production_str);
        
        if (action == 's') {  // Shift
            // Push the current symbol and next state onto stack
            top++;
            stack[top] = value;
            
            // Move to next input symbol
            input_pos++;
            current_symbol = input[input_pos];
        } else if (action == 'r') {  // Reduce
            // Get the production
            char lhs = productions_lhs[value];
            char *rhs = productions_rhs[value];
            int rhs_len = strlen(rhs);
            
            // Pop |rhs| states off the stack
            top -= rhs_len;
            
            // Get the state at the top of the reduced stack
            int state = stack[top];
            
            // Find the goto value
            int goto_state = -1;
            for (int nt = 0; nt < num_non_terminals; nt++) {
                if (non_terminals[nt] == lhs) {
                    goto_state = goto_destinations[state][lhs];
                    break;
                }
            }
            
            if (goto_state == -1) {
                printf("Error: No goto entry for %c from state %d\n", lhs, state);
                return 0;
            }
            
            // Push the LHS non-terminal and new state onto stack
            top++;
            stack[top] = goto_state;
        } else if (action == 'a') {  // Accept
            printf("\nInput accepted!\n");
            return 1;
        } else {  // Error
            printf("\nError: No action defined for state %d and symbol %c\n", current_state, current_symbol);
            return 0;
        }
    }
    
    return 0;
}

int main() {
    char input_string[MAX_STRING];
    
    printf("LALR Parser Implementation\n");
    printf("==========================\n\n");
    
    printf("Enter the number of productions: ");
    scanf("%d", &num_productions);
    getchar();  // Consume newline
    
    printf("Enter the productions (e.g., S->S+T):\n");
    for (int i = 0; i < num_productions; i++) {
        char production[MAX_SYMBOLS + 3];  // +3 for "->" and null terminator
        
        printf("Production %d: ", i + 1);
        fgets(production, sizeof(production), stdin);
        
        // Remove trailing newline
        production[strcspn(production, "\n")] = 0;
        
        // Parse the production
        if (sscanf(production, "%c->%s", &productions_lhs[i], productions_rhs[i]) != 2) {
            printf("Invalid production format. Please use the format 'X->abc'\n");
            i--;
            continue;
        }
        
        // Add non-terminal to the list
        add_non_terminal(productions_lhs[i]);
        
        // Add terminals and non-terminals from RHS
        for (int j = 0; productions_rhs[i][j] != '\0'; j++) {
            char symbol = productions_rhs[i][j];
            if (is_terminal(symbol)) {
                add_terminal(symbol);
            } else if (is_non_terminal(symbol)) {
                add_non_terminal(symbol);
            }
        }
    }
    
    // Augment the grammar with Z -> S
    productions_lhs[num_productions] = 'Z';
    sprintf(productions_rhs[num_productions], "%c", productions_lhs[0]);
    num_productions++;
    add_non_terminal('Z');
    
    // Construct the LALR parsing table
    construct_lalr_table();
    
    // Print the states and parsing table
    print_states();
    print_lalr_table();
    
    // Get input string to parse
    printf("\nEnter the input string to parse (e.g., id+id*id): ");
    fgets(input_string, sizeof(input_string), stdin);
    
    // Remove trailing newline
    input_string[strcspn(input_string, "\n")] = 0;
    
    // Parse the input string
    parse_input(input_string);
    
    return 0;
}