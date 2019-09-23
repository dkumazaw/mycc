#include "9cc.h"

/*
consume
*/
int consume(Vector *tokens, int ty)
{
    if (((Token *)tokens->data[pos])->ty != ty)
        return 0;
    pos++;
    return 1;
}

/*
program: stmt program
program: epsion
*/
void *program(Vector *tokens)
{
    int i = 0;
    while (((Token *)tokens->data[pos])->ty != TK_EOF)
        code[i++] = stmt(tokens);
    code[i] = NULL;
}

/*
assign: equality
assign: equality "=" assign
*/
Node *assign(Vector *tokens)
{
    Node *node = equality(tokens);
    while (consume(tokens, '='))
        node = new_node('=', node, assign(tokens));
    return node;
}

/*
expr: assign
*/
Node *expr(Vector *tokens)
{
    Node *node = assign(tokens);
    return node;
}

/*
stmt: "return" expr ";"
    | "while" "(" expr ")" stmt
    | "if" "(" expr ")" stmt ("else" stmt)?
    | "for" "(" expr? ";" expr? ";" expr? ")" stmt
    | "{" stmt* "}"
    | expr ";"
*/
Node *stmt(Vector *tokens)
{
    Node *node;
    node = malloc(sizeof(Node));

    if (consume(tokens, TK_RETURN))
    {
        node->ty = ND_RETURN;
        node->lhs = expr(tokens);
        if (!consume(tokens, ';'))
        {
            fprintf(stderr, "Expected ';' but got %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }
    }
    else if (consume(tokens, TK_WHILE)) 
    {    
        node->ty = ND_WHILE;
        if (!consume(tokens, '('))
        {
            fprintf(stderr, "No opening parenthesis: %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }
        node->cond = expr(tokens);
        if (!consume(tokens, ')'))
        {
            fprintf(stderr, "No closing parenthesis: %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }
        node->body = stmt(tokens);
    }
    else if (consume(tokens, TK_IF))
    {
        node->ty = ND_IF;
        if (!consume(tokens, '('))
        {
            fprintf(stderr, "No opening parenthesis: %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }
        node->cond = expr(tokens);
        if (!consume(tokens, ')'))
        {
            fprintf(stderr, "No closing parenthesis: %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }

        node->then = stmt(tokens);

        if (consume(tokens, TK_ELSE))
        {
            node->els = stmt(tokens);
        }
        else
        {
            // No else statement found,
            // set the els to null
            node->els = NULL;
        }
    }
    else if (consume(tokens, TK_FOR))
    {
        node->ty = ND_FOR;
        if (!consume(tokens, '('))
        {
            fprintf(stderr, "No opening parenthesis: %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }
        // init
        if (!consume(tokens, ';')) 
        {
            node->init = expr(tokens);
            consume(tokens, ';');
        }
        else 
        {
            node->init = NULL;
        }

        // cond
        if (!consume(tokens, ';'))
        {
            node->cond = expr(tokens);
            consume(tokens, ';');
        }
        else 
        {
            node->cond = NULL;
        }

        // then
        if (!consume(tokens, ';'))
        {
            node->then = expr(tokens);
            consume(tokens, ';');
        }
        else 
        {
            node->cond = NULL;
        }
        consume(tokens, ')');

        // body
        node->body = stmt(tokens);
    }
    else if (consume(tokens, '{')) 
    {
        // This is a block!
        node->ty = ND_BLOCK;
        // Initialize the vector
        node->stmts = new_vector();

        while (!consume(tokens, '}')) 
        {
            vec_push(node->stmts, (void *)stmt(tokens));
        }
    }
    else
    {
        node = expr(tokens);
        if (!consume(tokens, ';'))
        {
            fprintf(stderr, "Expected ';' but got %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }
    }

    return node;
}

/*
equality: relational
equality: equality "==" relational
equality: equality "!=" relational
*/
Node *equality(Vector *tokens)
{
    Node *node = relational(tokens);

    for (;;)
    {
        if (consume(tokens, TK_EQ))
            node = new_node(ND_EQ, node, relational(tokens));
        else if (consume(tokens, TK_NE))
            node = new_node(ND_NE, node, relational(tokens));
        else
            return node;
    }
}

/*
relational: add
relational: relational "<"  add
relational: relational "<=" add
relational: relational ">"  add
relational: relational ">=" add
*/
Node *relational(Vector *tokens)
{
    Node *node = add(tokens);

    for (;;)
    {
        if (consume(tokens, '<'))
            node = new_node('<', node, add(tokens));
        else if (consume(tokens, TK_LE))
            node = new_node(ND_LE, node, add(tokens));
        else if (consume(tokens, '>'))
            node = new_node('<', add(tokens), node); // Flip to represent <
        else if (consume(tokens, TK_GE))
            node = new_node(ND_LE, add(tokens), node); // Flip to represent <=
        else
            return node;
    }
}

/*
add: mul
add: mul "+" mul
add: mul "-" mul
*/
Node *add(Vector *tokens)
{
    Node *node = mul(tokens);

    for (;;)
    {
        if (consume(tokens, '+'))
            node = new_node('+', node, mul(tokens));
        else if (consume(tokens, '-'))
            node = new_node('-', node, mul(tokens));
        else
            return node;
    }
}

/*
mul: unary
mul: mul "*" unary
mul: mul "/" unary
*/
Node *mul(Vector *tokens)
{
    Node *node = unary(tokens);

    for (;;)
    {
        if (consume(tokens, '*'))
            node = new_node('*', node, unary(tokens));
        else if (consume(tokens, '/'))
            node = new_node('/', node, unary(tokens));
        else
            return node;
    }
}

/*
unary: term
unary: "+" term
unary: "-" term
*/
Node *unary(Vector *tokens)
{
    if (consume(tokens, '+'))
        return term(tokens);
    if (consume(tokens, '-'))
        return new_node('-', new_node_num(0), term(tokens));
    return term(tokens);
}

/*
term: num
term: ident
term: "(" assign ")"
*/
Node *term(Vector *tokens)
{
    if (consume(tokens, '('))
    {
        Node *node = assign(tokens);
        if (!consume(tokens, ')'))
        {
            fprintf(stderr, "No closing parenthesis: %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }
        return node;
    }

    if (((Token *)tokens->data[pos])->ty == TK_NUM)
        return new_node_num(((Token *)tokens->data[pos++])->val);

    if (((Token *)tokens->data[pos])->ty == TK_IDENT) {
        int offset;
        LVar *lvar = find_lvar(((Token *)tokens->data[pos]));
        if (lvar) {
            offset = lvar->offset;
        } else {
            lvar = malloc(sizeof(LVar));
            lvar->next = locals;
            lvar->name = ((Token *)tokens->data[pos])->input;
            lvar->len = ((Token *)tokens->data[pos])->len;
            lvar->offset = locals->offset + 8;
            offset = lvar->offset;
            locals = lvar;
        }
	    pos++;
        return new_node_ident(offset);
    }

    fprintf(stderr, "Token is neither an integer or parenthesis: %s", ((Token *)tokens->data[pos])->input);
    exit(1);
}
