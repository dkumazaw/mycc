#include "9cc.h"

void gen_lval(Node *node)
{
    if (node->ty != ND_IDENT)
    {
        fprintf(stderr, "Expected a variable on the LHS");
        exit(1);
    }

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n"); // Pushes the address of the target variable
}

// Generates assembler code
void gen(Node *node)
{
    if (node->ty == ND_NUM)
    {
        printf("  push %d\n", node->val);
        return;
    }

    if (node->ty == ND_RETURN)
    {
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    }

    // Case where variable appears on the RHS:
    // Want to treat it like a right value
    if (node->ty == ND_IDENT)
    {
        gen_lval(node); // Now the address of the target is stored on stack
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n"); // Retrieve val from the stack
        printf("  push rax\n");
        return;
    }

    if (node->ty == ND_WHILE) 
    {
        printf(".Lbegin%03d:\n", jump_count);
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lend%03d\n", jump_count);
        gen(node->body);
        printf("  jmp .Lbegin%03d\n", jump_count);
        printf(".Lend%03d:\n", jump_count);
        jump_count++;
        return;
    }
    
    if (node->ty == ND_FOR)
    {
        gen(node->init);
        printf(".Lbegin%03d:\n", jump_count);
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lend%03d\n", jump_count);
        gen(node->body);
        gen(node->then);
        printf("  jmp .Lbegin%03d\n", jump_count);
        printf(".Lend%03d:\n", jump_count);
        jump_count++;
        return;
    }

    if (node->ty == ND_IF)
    {
        if (!node->els) 
        {
            // No else statement
            gen(node->cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lend%03d\n", jump_count);
            gen(node->then);
            printf(".Lend%03d:\n", jump_count);
            jump_count++;       
            return;
        }
        else {
            // There is an else statement
            gen(node->cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lelse%03d\n", jump_count);
            gen(node->then);
            printf("  jmp .Lend%03d\n", jump_count);
            printf(".Lelse%03d:\n", jump_count);
            gen(node->els);
            printf(".Lend%03d:\n", jump_count);
            jump_count++;
            return;
        }
    }

    if (node->ty == ND_BLOCK)
    {
        for (int count = 0; count < node->stmts->len; count++) {
            gen((Node *)vec_get(node->stmts, count));
            printf("  pop rax\n");
        }
        return;
    }

    if (node->ty == '=')
    {
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->ty)
    {
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case '<':
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    case '+':
        printf("  add rax, rdi\n");
        break;
    case '-':
        printf("  sub rax, rdi\n");
        break;
    case '*':
        printf("  mul rdi\n");
        break;
    case '/':
        printf("  mov rdx, 0\n");
        printf("  div rdi\n");
    }

    printf("  push rax\n");
}