#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    TK_NUM = 256, // integer token
    TK_IDENT,     // Identifier
    TK_RETURN,    // return
    TK_EQ,        // ==
    TK_NE,        // !=
    TK_LE,        // <=
    TK_GE,        // >=
    TK_EOF,       // token representing end of input
};

enum
{
    ND_NUM = 256, // integer node type
    ND_IDENT,     // Idenfitifer
    ND_RETURN,    // return
    ND_EQ,        // ==
    ND_NE,        // !=
    ND_LE,        // <=
    ND_GE,        // >=
};

/*
struct Token

int ty: TK_NUM if the content is an integer,
        ASCII expression if the content is an operator like '+',
        TK_EOF if the content is the end of input
int val: set to the integer value if ty is TK_NUM
char *input: pointer to the start of the token
*/
typedef struct
{
    int ty;      // type of token
    int val;     // if ty is TK_NUM, store its value
    char *input; // token str (for error message)
} Token;

typedef struct Node
{
    int ty;           // type
    struct Node *lhs; // lhs
    struct Node *rhs; // rhs
    int val;          // Used only when ty is ND_NUM
    char name;        // Used only when ty is ND_IDENT
} Node;

typedef struct
{
    void **data;
    int capacity;
    int len;
} Vector;

// container.c
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_ident(char name);
Token *new_token();
Vector *new_vector();
void vec_push(Vector *vec, void *elem);

// parse.c
int consume(Vector *tokens, int ty);
void *program(Vector *tokens);
Node *assign(Vector *tokens);
Node *stmt(Vector *tokens);
Node *add(Vector *tokens);
Node *mul(Vector *tokens);
Node *unary(Vector *tokens);
Node *term(Vector *tokens);
Node *relational(Vector *tokens);
Node *equality(Vector *tokens);
void vec_push(Vector *vec, void *elem);

// codegen.c
void gen_lval(Node *node);
void gen(Node *node);

// tokenize.c
int is_alnum(char c);
void tokenize(char *p, Vector *tokens);

// main.c
extern int pos;
extern Node *code[100];
