#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    TK_NUM = 256, // integer token
    TK_IDENT,     // Identifier
    TK_RETURN,    // return
    TK_IF,        // if
    TK_ELSE,      // else
    TK_WHILE,     // while
    TK_FOR,       // for
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
    ND_CALL,      // function call
    ND_RETURN,    // return
    ND_IF,        // if
    ND_ELSE,      // else
    ND_WHILE,     // while
    ND_FOR,       // for
    ND_BLOCK,     // block
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
    int len;     // Stores the length of the token, used only for TK_IDENT
} Token;

typedef struct
{
    void **data;
    int capacity;
    int len;
} Vector;

typedef struct Node Node;

struct Node
{
    int ty;           // type
    Node *lhs; // lhs
    Node *rhs; // rhs
    int val;          // Used only when ty is ND_NUM
    int offset;        // Used only when ty is ND_IDENT

    // while ('cond') 'body'
    // if ('cond') 'then' else 'els
    // for ('init'; 'cond'; 'then') 'body
    Node *init;
    Node *cond;
    Node *body;
    Node *then;
    Node *els;

    // Used when a block needs to store a series of stmts
    Vector* stmts;

    // Used when ty is ND_CALL
    char *fnct_name;
};


typedef struct LVar LVar;

struct LVar
{
    LVar *next;
    char *name;
    int len;
    int offset;
};

/*
container.c
*/
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_ident(int offset);
Token *new_token();

// Vector
Vector *new_vector();
void vec_push(Vector *vec, void *elem);
void *vec_get(Vector *vec, int pos);

LVar *find_lvar(Token *token);

// parse.c
int consume(Vector *tokens, int ty);
void *program(Vector *tokens);
Node *assign(Vector *tokens);
Node *expr(Vector *tokens);
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
extern LVar *locals;
extern unsigned int jump_count;