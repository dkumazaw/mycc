#include "9cc.h"

/*
tokenize 
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

        if (strncmp(p, "==", 2) == 0)
        {
            Token *token = new_token();
            token->ty = TK_EQ;
            token->input = p;
            vec_push(tokens, (void *)token);
            p += 2;
            continue;
        }

        if (strncmp(p, "!=", 2) == 0)
        {
            Token *token = new_token();
            token->ty = TK_NE;
            token->input = p;
            vec_push(tokens, (void *)token);
            p += 2;
            continue;
        }

        if (strncmp(p, "<=", 2) == 0)
        {
            Token *token = new_token();
            token->ty = TK_LE;
            token->input = p;
            vec_push(tokens, (void *)token);
            p += 2;
            continue;
        }

        if (strncmp(p, ">=", 2) == 0)
        {
            Token *token = new_token();
            token->ty = TK_GE;
            token->input = p;
            vec_push(tokens, (void *)token);
            p += 2;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>' || *p == ';' || *p == '=')
        {
            Token *token = new_token();
            token->ty = *p;
            token->input = p;
            vec_push(tokens, (void *)token);
            p++;
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

        if (isdigit(*p))
        {
            Token *token = new_token();
            token->ty = TK_NUM;
            token->input = p;
            token->val = strtol(p, &p, 10);
            vec_push(tokens, (void *)token);
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