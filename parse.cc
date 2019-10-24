#include "9cc.h"

/*
get_token

A helper function that returns a pointer to the token at the given offset from the current pos.
Assumes a global variable pos, which indicates the current position of the token.
*/
Token *get_token(Vector *tokens, int offset) 
{
    return ((Token *)vec_get(tokens, pos + offset));
}

/*
consume
*/
bool consume(int ty)
//int consume(Vector *tokens, int ty)
{
    if ((*token_itr).ty != ty)
        return false;
    
    token_itr++;
    return true;
    /*
    if (get_token(tokens, 0)->ty != ty)
        return 0;
    pos++;
    return 1;*/
}

/*
expect
*/
void expect(int ty, char* expected)
{
    if (!consume(ty)) 
    {
        //fprintf(stderr, "Expected %s but got %s", expected, get_token(tokens, 0)->input);
        exit(1);
    }
}

/*
program: stmt program
program: epsion
*/
void program()
//void *program(Vector *tokens)
{
    int i = 0;
    while ((*token_itr).ty != TK_EOF)
        code[i++] = stmt();
    code[i] = NULL;
    /*
    int i = 0;
    while (((Token *)tokens->data[pos])->ty != TK_EOF)
        code[i++] = stmt(tokens);
    code[i] = NULL;
    */
}

/*
assign: equality
assign: equality "=" assign
*/
Node *assign()
//Node *assign(Vector *tokens)
{
    Node *node = equality();
    while (consume('='))
        node = new_node('=', node, assign());
    return node;
    // Node *node = equality(tokens);
    // while (consume(tokens, '='))
    //     node = new_node('=', node, assign(tokens));
    // return node;
}

/*
expr: assign
*/
Node *expr()
{
    Node *node = assign();
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
Node *stmt()
{
    Node *node;
    node = new Node;

    if (consume(TK_RETURN))
    {
        node->ty = ND_RETURN;
        node->lhs = expr();
        if (!consume(';'))
        {
            //fprintf(stderr, "Expected ';' but got %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }
    }
    else if (consume(TK_WHILE)) 
    {    
        node->ty = ND_WHILE;
        if (!consume('('))
        {
            //fprintf(stderr, "No opening parenthesis: %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }
        node->cond = expr();
        if (!consume(')'))
        {
            //fprintf(stderr, "No closing parenthesis: %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }
        node->body = stmt();
    }
    else if (consume(TK_IF))
    {
        node->ty = ND_IF;
        if (!consume('('))
        {
            //fprintf(stderr, "No opening parenthesis: %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }
        node->cond = expr();
        if (!consume(')'))
        {
            //fprintf(stderr, "No closing parenthesis: %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }

        node->then = stmt();

        if (consume(TK_ELSE))
        {
            node->els = stmt();
        }
        else
        {
            // No else statement found,
            // set the els to null
            node->els = NULL;
        }
    }
    else if (consume(TK_FOR))
    {
        node->ty = ND_FOR;
        if (!consume('('))
        {
            //fprintf(stderr, "No opening parenthesis: %s", ((Token *)tokens->data[pos])->input);
            exit(1);
        }
        // init
        if (!consume(';')) 
        {
            node->init = expr();
            consume(';');
        }
        else 
        {
            node->init = NULL;
        }

        // cond
        if (!consume(';'))
        {
            node->cond = expr();
            consume(';');
        }
        else 
        {
            node->cond = NULL;
        }

        // then
        if (!consume(';'))
        {
            node->then = expr();
            consume(';');
        }
        else 
        {
            node->cond = NULL;
        }
        consume(')');

        // body
        node->body = stmt();
    }
    else if (consume('{')) 
    {
        // This is a block!
        node->ty = ND_BLOCK;
        // Initialize the vector
        node->stmts = new_vector();

        while (!consume('}')) 
        {
            vec_push(node->stmts, (void *)stmt());
        }
    }
    else
    {
        node = expr();
        if (!consume(';'))
        {
            //fprintf(stderr, "Expected ';' but got %s", ((Token *)tokens->data[pos])->input);
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
Node *equality()
{
    Node *node = relational();

    for (;;)
    {
        if (consume(TK_EQ))
            node = new_node(ND_EQ, node, relational());
        else if (consume(TK_NE))
            node = new_node(ND_NE, node, relational());
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
Node *relational()
{
    Node *node = add();

    for (;;)
    {
        if (consume('<'))
            node = new_node('<', node, add());
        else if (consume(TK_LE))
            node = new_node(ND_LE, node, add());
        else if (consume('>'))
            node = new_node('<', add(), node); // Flip to represent <
        else if (consume(TK_GE))
            node = new_node(ND_LE, add(), node); // Flip to represent <=
        else
            return node;
    }
}

/*
add: mul
add: mul "+" mul
add: mul "-" mul
*/
Node *add()
{
    Node *node = mul();

    for (;;)
    {
        if (consume('+'))
            node = new_node('+', node, mul());
        else if (consume('-'))
            node = new_node('-', node, mul());
        else
            return node;
    }
}

/*
mul: unary
mul: mul "*" unary
mul: mul "/" unary
*/
Node *mul()
{
    Node *node = unary();

    for (;;)
    {
        if (consume('*'))
            node = new_node('*', node, unary());
        else if (consume('/'))
            node = new_node('/', node, unary());
        else
            return node;
    }
}

/*
unary: term
unary: "+" term
unary: "-" term
*/
Node *unary()
{
    if (consume('+'))
        return term();
    if (consume('-'))
        return new_node('-', new_node_num(0), term());
    return term();
}

/*
term: num
    | ident ( "(" ")" )?
    | "(" assign ")"
*/
Node *term()
{
    if (consume('('))
    {
        Node *node = assign();
        expect(')', ")");
        return node;
    }

    if ((*token_itr).ty == TK_NUM)
        return new_node_num((*(token_itr++)).val);

    if ((*token_itr).ty == TK_IDENT) {
        Token& current_token = *token_itr;
        Token& next_token = *(token_itr + 1);

        if (next_token.ty == '(') {
            // Found a function!
            Node *node = new Node;
            node->ty = ND_CALL;
            char *function_name = (char *)malloc(current_token.len);
            strncpy(function_name, current_token.input, current_token.len); // Copy name
            node->fnct_name = function_name;
            pos++;
            consume('(');
            expect(')', ")");
            return node;
        }   
        else 
        {     
            // Found a variable!
            int offset;
            LVar *lvar = find_lvar();
            if (lvar) {
                offset = lvar->offset;
            } else {
                lvar = (LVar *)malloc(sizeof(LVar));
                lvar->next = locals;
                lvar->name = current_token.input;
                lvar->len = current_token.len;
                lvar->offset = locals->offset + 8;
                offset = lvar->offset;
                locals = lvar;
            }
	        pos++;
            return new_node_ident(offset);
        }
    }

    //fprintf(stderr, "Token is neither an integer or parenthesis: %s", get_token(tokens, 0)->input);
    exit(1);
}
