#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_EXPR_LEN 100
#define MAX_TOKENS 100
#define MAX_CODE_LINES 100

typedef enum {
    OPERAND,
    OPERATOR,
    LEFT_PAREN,
    RIGHT_PAREN
} TokenType;

typedef struct {
    TokenType type;
    char value[20];
} Token;

typedef struct {
    Token tokens[MAX_TOKENS];
    int count;
} TokenArray;

typedef struct {
    char result[20];
    char op[5];
    char arg1[20];
    char arg2[20];
} ThreeAddressCode;

typedef struct {
    ThreeAddressCode code[MAX_CODE_LINES];
    int count;
} ThreeAddressCodeArray;

typedef struct {
    char op[5];      // Operator
    char arg1[20];   // Operand 1
    char arg2[20];   // Operand 2
    char result[20]; // Result
} Quadruple;

typedef struct {
    Quadruple quads[MAX_CODE_LINES];
    int count;
} QuadrupleArray;

typedef struct {
    char op[5];     // Operator
    char arg1[20];  // Operand 1
    char arg2[20];  // Operand 2
} Triple;

typedef struct {
    Triple triples[MAX_CODE_LINES];
    int count;
} TripleArray;

typedef struct {
    int indices[MAX_CODE_LINES];
    int count;
} IndirectTripleArray;

int isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%';
}

int getPrecedence(char op) {
    switch (op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
        case '%':
            return 2;
        default:
            return 0;
    }
}

void tokenize(char *expr, TokenArray *tokens) {
    int i = 0;
    int token_index = 0;
    
    while (expr[i] != '\0' && token_index < MAX_TOKENS) {
        if (isspace(expr[i])) {
            i++;
            continue;
        }
        
        if (isdigit(expr[i]) || isalpha(expr[i])) {
            int j = 0;
            Token token;
            token.type = OPERAND;
            
            while ((isdigit(expr[i]) || isalpha(expr[i])) && j < 19) {
                token.value[j++] = expr[i++];
            }
            token.value[j] = '\0';
            tokens->tokens[token_index++] = token;
        } else if (isOperator(expr[i])) {
            Token token;
            token.type = OPERATOR;
            token.value[0] = expr[i];
            token.value[1] = '\0';
            tokens->tokens[token_index++] = token;
            i++;
        } else if (expr[i] == '(') {
            Token token;
            token.type = LEFT_PAREN;
            token.value[0] = expr[i];
            token.value[1] = '\0';
            tokens->tokens[token_index++] = token;
            i++;
        } else if (expr[i] == ')') {
            Token token;
            token.type = RIGHT_PAREN;
            token.value[0] = expr[i];
            token.value[1] = '\0';
            tokens->tokens[token_index++] = token;
            i++;
        } else {
            i++;
        }
    }
    
    tokens->count = token_index;
}

void infixToPostfix(TokenArray *infix, TokenArray *postfix) {
    Token stack[MAX_TOKENS];
    int top = -1;
    int i, j = 0;
    
    for (i = 0; i < infix->count; i++) {
        Token token = infix->tokens[i];
        
        switch (token.type) {
            case OPERAND:
                postfix->tokens[j++] = token;
                break;
            case LEFT_PAREN:
                stack[++top] = token;
                break;
            case RIGHT_PAREN:
                while (top > -1 && stack[top].type != LEFT_PAREN) {
                    postfix->tokens[j++] = stack[top--];
                }
                if (top > -1 && stack[top].type == LEFT_PAREN) {
                    top--;
                }
                break;
            case OPERATOR:
                while (top > -1 && stack[top].type == OPERATOR &&
                       getPrecedence(stack[top].value[0]) >= getPrecedence(token.value[0])) {
                    postfix->tokens[j++] = stack[top--];
                }
                stack[++top] = token;
                break;
        }
    }
    
    while (top > -1) {
        postfix->tokens[j++] = stack[top--];
    }
    
    postfix->count = j;
}

void generateThreeAddressCode(TokenArray *postfix, ThreeAddressCodeArray *code) {
    Token stack[MAX_TOKENS];
    int top = -1;
    int temp_var = 1;
    int i;
    
    for (i = 0; i < postfix->count; i++) {
        Token token = postfix->tokens[i];
        
        if (token.type == OPERAND) {
            stack[++top] = token;
        } else if (token.type == OPERATOR) {
            ThreeAddressCode tac;
            Token op2 = stack[top--];
            Token op1 = stack[top--];
            
            sprintf(tac.arg1, "%s", op1.value);
            sprintf(tac.arg2, "%s", op2.value);
            sprintf(tac.op, "%s", token.value);
            sprintf(tac.result, "t%d", temp_var);
            
            code->code[code->count++] = tac;
            
            Token result_token;
            result_token.type = OPERAND;
            sprintf(result_token.value, "t%d", temp_var);
            stack[++top] = result_token;
            
            temp_var++;
        }
    }
}

void generateQuadruples(TokenArray *postfix, QuadrupleArray *quads) {
    Token stack[MAX_TOKENS];
    int top = -1;
    int temp_var = 1;
    int i;
    
    for (i = 0; i < postfix->count; i++) {
        Token token = postfix->tokens[i];
        
        if (token.type == OPERAND) {
            stack[++top] = token;
        } else if (token.type == OPERATOR) {
            Quadruple quad;
            Token op2 = stack[top--];
            Token op1 = stack[top--];
            
            strcpy(quad.op, token.value);
            strcpy(quad.arg1, op1.value);
            strcpy(quad.arg2, op2.value);
            sprintf(quad.result, "t%d", temp_var);
            
            quads->quads[quads->count++] = quad;
            
            Token result_token;
            result_token.type = OPERAND;
            sprintf(result_token.value, "t%d", temp_var);
            stack[++top] = result_token;
            
            temp_var++;
        }
    }
}

void generateTriples(TokenArray *postfix, TripleArray *triples) {
    Token stack[MAX_TOKENS];
    int top = -1;
    int i;
    
    for (i = 0; i < postfix->count; i++) {
        Token token = postfix->tokens[i];
        
        if (token.type == OPERAND) {
            stack[++top] = token;
        } else if (token.type == OPERATOR) {
            Triple triple;
            Token op2 = stack[top--];
            Token op1 = stack[top--];
            
            strcpy(triple.op, token.value);
            strcpy(triple.arg1, op1.value);
            strcpy(triple.arg2, op2.value);
            
            triples->triples[triples->count++] = triple;
            
            Token result_token;
            result_token.type = OPERAND;
            sprintf(result_token.value, "(%d)", triples->count - 1);
            stack[++top] = result_token;
        }
    }
}

void generateIndirectTriples(TokenArray *postfix, TripleArray *triples, IndirectTripleArray *indirect) {
    generateTriples(postfix, triples);
    
    // Generate a list of indices in the order operations should be performed
    for (int i = 0; i < triples->count; i++) {
        indirect->indices[indirect->count++] = i;
    }
}

void printThreeAddressCode(ThreeAddressCodeArray *code) {
    printf("\n=== Three Address Code ===\n");
    for (int i = 0; i < code->count; i++) {
        ThreeAddressCode tac = code->code[i];
        printf("%s = %s %s %s\n", tac.result, tac.arg1, tac.op, tac.arg2);
    }
}

void printQuadruples(QuadrupleArray *quads) {
    printf("\n=== Quadruples ===\n");
    printf("Index\tOperator\tArg1\tArg2\tResult\n");
    printf("-----\t--------\t----\t----\t------\n");
    for (int i = 0; i < quads->count; i++) {
        Quadruple q = quads->quads[i];
        printf("(%d)\t%s\t\t%s\t%s\t%s\n", i, q.op, q.arg1, q.arg2, q.result);
    }
}

void printTriples(TripleArray *triples) {
    printf("\n=== Triples ===\n");
    printf("Index\tOperator\tArg1\tArg2\n");
    printf("-----\t--------\t----\t----\n");
    for (int i = 0; i < triples->count; i++) {
        Triple t = triples->triples[i];
        printf("(%d)\t%s\t\t%s\t%s\n", i, t.op, t.arg1, t.arg2);
    }
}

void printIndirectTriples(TripleArray *triples, IndirectTripleArray *indirect) {
    printf("\n=== Indirect Triples ===\n");
    printf("Index\tPointer\tOperator\tArg1\tArg2\n");
    printf("-----\t-------\t--------\t----\t----\n");
    for (int i = 0; i < indirect->count; i++) {
        int idx = indirect->indices[i];
        Triple t = triples->triples[idx];
        printf("(%d)\t(%d)\t%s\t\t%s\t%s\n", i, idx, t.op, t.arg1, t.arg2);
    }
}

int main() {
    char expr[MAX_EXPR_LEN];
    TokenArray infix = {0};
    TokenArray postfix = {0};
    ThreeAddressCodeArray code = {0};
    QuadrupleArray quads = {0};
    TripleArray triples = {0};
    IndirectTripleArray indirect = {0};
    
    printf("Enter an arithmetic expression: ");
    fgets(expr, MAX_EXPR_LEN, stdin);
    
    // Remove newline if present
    size_t len = strlen(expr);
    if (len > 0 && expr[len-1] == '\n') {
        expr[len-1] = '\0';
    }
    
    printf("\nInput expression: %s\n", expr);
    
    tokenize(expr, &infix);
    
    printf("\nTokens: ");
    for (int i = 0; i < infix.count; i++) {
        printf("%s ", infix.tokens[i].value);
    }
    printf("\n");
    
    infixToPostfix(&infix, &postfix);
    
    printf("\nPostfix notation: ");
    for (int i = 0; i < postfix.count; i++) {
        printf("%s ", postfix.tokens[i].value);
    }
    printf("\n");
    
    // Generate different intermediate representations
    generateThreeAddressCode(&postfix, &code);
    generateQuadruples(&postfix, &quads);
    generateIndirectTriples(&postfix, &triples, &indirect);
    
    // Print all representations
    printThreeAddressCode(&code);
    printQuadruples(&quads);
    printTriples(&triples);
    printIndirectTriples(&triples, &indirect);
    
    return 0;
}