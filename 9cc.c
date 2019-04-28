#include "9cc.h"

// Function prototypes
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Vector *new_vector();
void vec_push(Vector *vec, void *elem);
int consume(Vector *tokens, int ty);
Node *add();
Node *mul();
Node *term();
void gen(Node *node);
void tokenize(char *p, Vector *tokens);
//void error(int i);

int pos = 0;     // Stores current position when parsing tokens
Node *code[100]; // TODO: replace this with a vector

/*
Creates a new node w/ lhr and rhs
*/
Node *new_node(int ty, Node *lhs, Node *rhs)
{
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

/*
Creates a number node
*/
Node *new_node_num(int val)
{
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

/* 
Creates a ident node
*/
Node *new_node_ident(char name)
{
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->name = name;
    return node;
}

/*
Token *new_token

Creates a new token
*/
Token *new_token()
{
    Token *token = malloc(sizeof(Token));
    return token;
}

/*
Vector *new_vector

Creates a new vector and initializes its capacity to 16.
*/
Vector *new_vector()
{
    Vector *vec = malloc(sizeof(Vector));
    vec->data = malloc(sizeof(void *) * 16);
    vec->capacity = 16;
    vec->len = 0;
    return vec;
}

/*
Vector *new_vector

Pushes a new element to vec.
*/
void vec_push(Vector *vec, void *elem)
{
    if (vec->capacity == vec->len)
    {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
    }
    vec->data[vec->len++] = elem;
}

/*
consume

Checks if the current position of the vector is of type ty
*/
int consume(Vector *tokens, int ty)
{
    if (((Token *)tokens->data[pos])->ty != ty)
        return 0;
    pos++;
    return 1;
}

/*
add: mul
add: add "+" mul
add: add "-" mul
*/
Node *add(Vector *tokens)
{
    Node *node = mul(tokens);

    for (;;)
    {
        if (consume(tokens, '+'))
            node = new_node('+', node, mul(tokens));
        else if (consume(tokens, '-'))
            node = new_node('-', node, mul(tokens));
        else
            return node;
    }
}

/*
mul: term
mul: mul "*" term
mul: mul "/" term
*/
Node *mul(Vector *tokens)
{
    Node *node = term(tokens);

    for (;;)
    {
        if (consume(tokens, '*'))
            node = new_node('*', node, term(tokens));
        else if (consume(tokens, '/'))
            node = new_node('/', node, term(tokens));
        else
            return node;
    }
}

/*
term: num
term: ident
term: "(" assign ")"
*/
Node *term(Vector *tokens)
{
    if (consume(tokens, '('))
    {
        Node *node = add(tokens);
        if (!consume(tokens, ')'))
        {
            fprintf(stderr, "No closing parenthesis: %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }
        return node;
    }

    if (((Token *)tokens->data[pos])->ty == TK_NUM)
        return new_node_num(((Token *)tokens->data[pos++])->val);

    if (((Token *)tokens->data[pos])->ty == TK_IDENT)
        return new_node_ident(*(((Token *)tokens->data[pos++])->input));

    fprintf(stderr, "Token is neither an integer or parenthesis: %s", ((Token *)tokens->data[pos])->input);
    exit(1);
}

/*
assign: add
assign: add "=" assign
*/
Node *assign(Vector *tokens)
{
    Node *node = add(tokens);
    while (consume(tokens, '='))
        node = new_node('=', node, assign(tokens));
    return node;
}

/*
stmt: assign ";"
*/
Node *stmt(Vector *tokens)
{
    Node *node = assign(tokens);
    if (!consume(tokens, ';'))
    {
        fprintf(stderr, "Invalid token found: expected ; but got %s", ((Token *)tokens->data[pos])->input);
        exit(1);
    }

    return node;
}

/*
program: stmt program
program: epsilon
*/
void program(Vector *tokens)
{
    int i = 0;
    while (((Token *)tokens->data[pos])->ty != TK_EOF)
        code[i++] = stmt(tokens);
    code[i] = NULL;
}

// Generates assembler code
void gen(Node *node)
{
    if (node->ty == ND_NUM)
    {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->ty)
    {
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

/*
tokenize 

Tokenizes the given string and stores the tokens in a vector
*/
void tokenize(char *p, Vector *tokens)
{
    while (*p)
    {
        // Skip whitespace
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')')
        {
            Token *token = new_token();
            token->ty = *p;
            token->input = p;
            vec_push(tokens, (void *)token);
            p++;
            continue;
        }

        if (isdigit(*p))
        {
            Token *token = new_token();
            token->ty = TK_NUM;
            token->input = p;
            token->val = strtol(p, &p, 10);
            vec_push(tokens, (void *)token);
            continue;
        }

        if ('a' <= *p && *p <= 'z')
        {
            Token *token = new_token();
            token->ty = TK_IDENT;
            token->input = p;
            vec_push(tokens, (void *)token);
            p++;
            continue;
        }

        fprintf(stderr, "Cannot tokenize: %s\n", p);
        exit(1);
    }

    Token *token = new_token();
    token->ty = TK_EOF;
    token->input = p;
    vec_push(tokens, (void *)token);
}

int expect(int line, int expected, int actual)
{
    if (expected == actual)
        return 0;
    fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
    exit(1);
}

void runtest()
{
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
//void error(int i)
//{
//    fprintf(stderr, "Unexpected token: %d\n",
//            tokens[i].input);
//    exit(1);
//}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Incorrect number of arguments\n");
        return 1;
    }

    if (strcmp(argv[1], "-test") == 0)
    {
        runtest();
        return 0;
    }

    Vector *tokens = new_vector(); // Initialize a token vector
    // Tokenize input and parse
    tokenize(argv[1], tokens);
    Node *node = add(tokens);

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
