#include "9cc.h"

int pos = 0;
Node *code[100];
size_t num_idents = 0;
Map *idents_to_offsets;

int expect(int line, int expected, int actual)
{
    if (expected == actual)
        return 0;
    fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
    exit(1);
}

void test_map()
{
    Map *map = new_map();
    expect(__LINE__, 0, (long)map_get(map, "foo"));

    map_put(map, "foo", (void *)2);
    expect(__LINE__, 2, (long)map_get(map, "foo"));

    map_put(map, "bar", (void *)4);
    expect(__LINE__, 4, (long)map_get(map, "bar"));

    map_put(map, "foo", (void *)6);
    expect(__LINE__, 6, (long)map_get(map, "foo")); // Scanned from bottom, so should see 6
}

void test_vector()
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
        test_vector();
        test_map();
        return 0;
    }

    idents_to_offsets = new_map(); // Initialize an identifier map
    Vector *tokens = new_vector(); // Initialize a token vector
    // Tokenize input and parse
    tokenize(argv[1], tokens);
    program(tokens);

    // Output the first part of assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // Prolog
    // Get enough space for the variables
    size_t variable_bytes = num_idents * 8;
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %ld\n", variable_bytes);

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