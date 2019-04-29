#include "9cc.h"

int pos = 0;

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
    Node *node = equality(tokens);

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