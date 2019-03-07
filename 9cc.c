#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    TK_NUM = 256, // integer token
    TK_EOF, // token representing end of input
};

typedef struct {
    int ty; // type of token
    int val; // if ty is TK_NUM, store its value
    char *input; // token str (for error message)
} Token;

// Tokenized tokens will be stored here
// Expect only up to 100 tokens
Token tokens[100];

// Tokenize the string pointed by p and store in tokens
void tokenize(char *p) {
    int i = 0;
    while (*p) {
        // Skip whitespace
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
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

    // Tokenize input
    tokenize(argv[1]);

    // Output the first part of assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // Check that the first input is number
    // and execute the first mov instruction
    if (tokens[0].ty != TK_NUM)
        error(0);
    printf("  mov rax, %d\n", tokens[0].val);

    int i = 1;
    while (tokens[i].ty != TK_EOF) {
        if (tokens[i].ty == '+') {
            i++;
            if (tokens[i].ty != TK_NUM)
                error(i);
            printf("  add rax, %d\n", tokens[i].val);
            i++;
            continue;
        }

        if (tokens[i].ty == '-') {
            i++;
            if (tokens[i].ty != TK_NUM)
                error(i);
            printf("  sub rax, %d\n", tokens[i].val);
            i++;
            continue;
        }

        error(i);
    }

    printf("  ret\n");
    return 0;
}