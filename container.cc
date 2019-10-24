#include "9cc.h"

Node *new_node(int ty, Node *lhs, Node *rhs)
{
    Node *node = new Node;
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val)
{
    Node *node = new Node;
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

Node *new_node_ident(int offset)
{
    Node *node = new Node;
    node->ty = ND_IDENT;
    node->offset = offset;
    return node;
}

/*
Token *new_token

Creates a new token
*/
Token *new_token()
{
    Token *token = new Token;
    return token;
}

/*
Vector *new_vector

Creates a new vector and initializes its capacity to 16.
*/
Vector *new_vector()
{
    Vector *vec = (Vector *)malloc(sizeof(Vector));
    vec->data = (void **)malloc(sizeof(void *) * 16);
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
        vec->data = (void **)realloc(vec->data, sizeof(void *) * vec->capacity);
    }
    vec->data[vec->len++] = elem;
}

/*
void* vec_get

Gets an element of the vector at the specified position
*/
void *vec_get(Vector *vec, int pos) 
{
    // Sanity check
    if (pos >= vec->len) {
        printf("Vector index out of range");
        exit(1);
    }

    return vec->data[pos];
}

LVar *find_lvar() {
    if (!locals)
	return NULL;
    for (LVar *var = locals; var; var = var->next) {
        if (strncmp(var->name, (*token_itr).input, var->len) == 0) {
            // A match was found!
            return var;
        }
    }
    return NULL;
}
