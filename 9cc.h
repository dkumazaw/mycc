#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct Node {
    int ty; // operator or ND_NUM
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;