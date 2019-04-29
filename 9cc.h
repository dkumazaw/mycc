#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    TK_NUM = 256, // integer token
    TK_EQ,        // ==
    TK_NE,        // !=
    TK_LE,        // <=
    TK_GE,        // >=
    TK_EOF,       // token representing end of input
};

enum
{
    ND_NUM = 256, // integer node type
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
    int ty; // operator or ND_NUM
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

typedef struct
{
    void **data;
    int capacity;
    int len;
} Vector;