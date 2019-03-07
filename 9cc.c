#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pos = 0;

enum {
    TK_NUM = 256, // integer token
    TK_EOF, // token representing end of input
};

enum {
    ND_NUM = 256, // integer node type
};

typedef struct {
    int ty; // type of token
    int val; // if ty is TK_NUM, store its value
    char *input; // token str (for error message)
} Token;

// Tokenized tokens will be stored here
// Expect only up to 100 tokens
Token tokens[100];

typedef struct Node {
    int ty; // operator or ND_NUM
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

Node *new_node(int ty, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

int consume(int ty) {
    if (tokens[pos].ty != ty)
        return 0;
    pos++;
    return 1;
}

Node *add() {
    Node *node = mul();

    for (;;) {
        if(consume('+'))
            node = new_node('+', node, mul());
        else if (consume('-'))
            node = new_node('-', node, mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = term();

    for (;;) {
        if (consume('*'))
            node = new_node('*', node, term());
        else if (consume('/'))
            node = new_node('/', node, term());
        else
            return node;
    }
}

Node *term() {
    if (consume('(')) {
        Node *node = add();
        if (!consume(')'))
            error("No closing parenthesis: %s", tokens[pos].input);
        return node;
    }

    if (tokens[pos].ty == TK_NUM)
        return new_node_num(tokens[pos++].val);
    
    error("Token is neither an integer or parenthesis: %s", tokens[pos].input);
}

void gen(Node *node) {
    if (node->ty == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->ty) {
    case '+':
        printf("  add rax, rdi\n");
        break;
    case '-':
        printf("  sub rax, rdi\n");
        break;
    case '*':
        printf("  mul rdi\n");
        break;
    case '/':
        printf("  mov rdx, 0\n");
        printf("  div rdi\n");
    }
    
    printf("  push rax\n");
}


// Tokenize the string pointed by p and store in tokens
void tokenize(char *p) {
    int i = 0;
    while (*p) {
        // Skip whitespace
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        fprintf(stderr, "Cannot tokenize: %s\n", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

// Reports error
void error(int i) {
    fprintf(stderr, "Unexpected token: %d\n",
                tokens[i].input);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect number of arguments\n");
        return 1;
    }

    // Tokenize input and parse 
    tokenize(argv[1]);
    Node *node = add();

    // Output the first part of assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // Descend the tree and generate code
    gen(node);

    // Fetch the last value on the stack
    // and load it to rax
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}