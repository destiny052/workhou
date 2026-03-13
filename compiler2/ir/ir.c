#include "ir.h"
#include "parser/ast.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


static int temp_id = 0;
static int label_id = 0;


static char *new_temp()
{
    static char buf[32];
    sprintf(buf, "t%d", temp_id++);
    return strdup(buf);
}

static char *new_label()
{
    static char buf[32];
    sprintf(buf, "L%d", label_id++);
    return strdup(buf);
}

void emit(IRList *ir, int op, const char *a1, const char *a2, const char *res)
{
    IRNode *node = (IRNode *)malloc(sizeof(IRNode));
    node->op = op;
    strcpy(node->arg1, a1 ? a1 : "-");
    strcpy(node->arg2, a2 ? a2 : "-");
    strcpy(node->result, res ? res : "-");
    node->next = NULL;

    if (!ir->head)
    {
        ir->head = node;
    }
    else
    {
        IRNode *p = ir->head;
        while (p->next)
            p = p->next;
        p->next = node;
    }
}


/* 修改gen_expr函数 */
static char *gen_expr(ASTNode *node, IRList *ir)
{
    if (!node) return NULL;
    
    if (node->type == AST_NUM) {
        return strdup(node->value);
    }
    
    if (node->type == AST_REAL) {
        static char buf[32];
        sprintf(buf, "%f", node->const_val.double_val);
        return strdup(buf);
    }
    
    if (node->type == AST_ID) {
        return strdup(node->value);
    }
    
    if (node->type == AST_BINOP) {
        char *a1 = gen_expr(node->left, ir);
        char *a2 = gen_expr(node->right, ir);
        char *t = new_temp();
        
        int op;
        // 根据操作数类型选择运算符
        if (node->left->data_type == INT_TYPE && node->right->data_type == INT_TYPE) {
            // 整数运算
            if (strcmp(node->value, "+") == 0) op = OP_ADD;
            else if (strcmp(node->value, "-") == 0) op = OP_SUB;
            else if (strcmp(node->value, "*") == 0) op = OP_MUL;
            else if (strcmp(node->value, "/") == 0) op = OP_DIV;
            else if (strcmp(node->value, "%") == 0) op = OP_MOD;
            else op = OP_ADD;
        } else {
            // 浮点运算
            if (strcmp(node->value, "+") == 0) op = OP_FADD;
            else if (strcmp(node->value, "-") == 0) op = OP_FSUB;
            else if (strcmp(node->value, "*") == 0) op = OP_FMUL;
            else if (strcmp(node->value, "/") == 0) op = OP_FDIV;
            else op = OP_FADD;
        }
        
        emit(ir, op, a1, a2, t);
        return t;
    }
    
    if (node->type == AST_TYPE_CAST) {
        char *src = gen_expr(node->left, ir);
        char *t = new_temp();
        int op;
        
        if (strcmp(node->value, "int") == 0) {
            op = OP_FLOAT_TO_INT;
        } else if (strcmp(node->value, "float") == 0 || strcmp(node->value, "double") == 0) {
            op = OP_INT_TO_FLOAT;
        } else {
            op = OP_ASSIGN;
        }
        
        emit(ir, op, src, "-", t);
        return t;
    }
    
    return NULL;
}


static void gen_stmt(ASTNode *node, IRList *ir)
{
    while (node)
    {
        switch (node->type)
        {
        case AST_ASSIGN:
        {
            char *rhs = gen_expr(node->right, ir);
            if (rhs && node->left)
            {
                emit(ir, OP_ASSIGN, rhs, "-", node->left->value);
            }
            break;
        }

        case AST_IF:
        {
            char *cond = gen_expr(node->cond, ir);
            char *Lend = new_label();
            emit(ir, OP_IF_FALSE, cond, "-", Lend);
            gen_stmt(node->body, ir);
            emit(ir, OP_LABEL, "-", "-", Lend);
            break;
        }

        case AST_WHILE:
        {
            char *Lbegin = new_label();
            char *Lend = new_label();
            emit(ir, OP_LABEL, "-", "-", Lbegin);
            char *cond = gen_expr(node->cond, ir);
            emit(ir, OP_IF_FALSE, cond, "-", Lend);
            gen_stmt(node->body, ir);
            emit(ir, OP_GOTO, "-", "-", Lbegin);
            emit(ir, OP_LABEL, "-", "-", Lend);
            break;
        }

        case AST_FOR:
        {
            char *Lbegin = new_label();
            char *Lend = new_label();
            gen_stmt(node->init, ir);
            emit(ir, OP_LABEL, "-", "-", Lbegin);
            char *cond = gen_expr(node->cond, ir);
            emit(ir, OP_IF_FALSE, cond, "-", Lend);
            gen_stmt(node->body, ir);
            gen_stmt(node->step, ir);
            emit(ir, OP_GOTO, "-", "-", Lbegin);
            emit(ir, OP_LABEL, "-", "-", Lend);
            break;
        }

        default:
            
            break;
        }
        node = node->next;
    }
}


IRList *gen_ir(ASTNode *root)
{
    IRList *ir = (IRList *)malloc(sizeof(IRList));
    ir->head = NULL;
    memset(ir->use_count, 0, sizeof(ir->use_count));


    if (root && root->type == AST_PROGRAM)
    {
        gen_stmt(root->left, ir);
    }
    else
    {
        gen_stmt(root, ir);
    }
    return ir;
}
