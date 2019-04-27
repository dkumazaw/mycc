#include "9cc.h"

// Function prototypes
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Vector *new_vector();
void vec_push(Vector *vec, void *elem);
int consume(int ty);
Node *add();
Node *mul();
Node *term();
void gen(Node *node);
void tokenize(char *p);
void error(int i);

// Tokenized tokens will be stored here
// Expect only up to 100 tokens

Vector token_vector = new_vector();
Token tokens[100];

int pos = 0;

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

// Creates a new vector
Vector *new_vector() {
    Vector *vec = malloc(sizeof(Vector));
    vec->data = malloc(sizeof(void *) * 16);
    vec->capacity = 16;
    vec->len = 0;
    return vec;
}

// Pushes an element to the vector
void vec_push(Vector *vec, void *elem) {
    if (vec->capacity == vec->len) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
    }
    vec->data[vec->len++] = elem;
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
        if (!consume(')')) {
            fprintf(stderr, "No closing parenthesis: %s", tokens[pos].input);
            exit(1);
        }
        return node;
    }

    if (tokens[pos].ty == TK_NUM)
        return new_node_num(tokens[pos++].val);
    
    fprintf(stderr, "Token is neither an integer or parenthesis: %s", tokens[pos].input);
    exit(1);
}

// Generates assembler code
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

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
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

int expect(int line, int expected, int actual) {
    if (expected == actual)
        return 0;
    fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
    exit(1);
}

void runtest() {
    Vector *vec = new_vector();
    expect(__LINE__, 0, vec->len);

    for (int i = 0; i < 100; i++)
        vec_push(vec, (void *)i);
    
    expect(__LINE__, 100, vec->len);
    expect(__LINE__, 0, (int)vec->data[0]);
    expect(__LINE__, 50, (int)vec->data[50]);
    expect(__LINE__, 99, (int)vec->data[99]);

    printf("OK\n");
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

    if (strcmp(argv[1], "-test") == 0) {
        runtest();
        return 0;
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
