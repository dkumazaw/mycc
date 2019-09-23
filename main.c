#include "9cc.h"

int pos = 0;
Node *code[100];
LVar *locals;
unsigned int jump_count = 0;

int check(int line, int expected, int actual)
{
    if (expected == actual)
        return 0;
    fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
    exit(1);
}

void runtest()
{
    Vector *vec = new_vector();
    check(__LINE__, 0, vec->len);

    for (int i = 0; i < 100; i++)
        vec_push(vec, (void *)i);

    check(__LINE__, 100, vec->len);
    check(__LINE__, 0, (int)vec->data[0]);
    check(__LINE__, 50, (int)vec->data[50]);
    check(__LINE__, 99, (int)vec->data[99]);

    printf("OK\n");
}

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
    LVar *lvar = malloc(sizeof(LVar)); // Initialize locals 
    lvar->offset = 0;
    lvar->name = "INIT";
    locals = lvar;
    // Tokenize input and parse
    tokenize(argv[1], tokens);
    program(tokens);

    // Output the first part of assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // Prolog
    // Get enough space for 26 variables
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    for (int i = 0; code[i]; i++)
    {
        gen(code[i]);
        printf("  pop rax\n");
    }

    // Epilog
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return 0;
}
