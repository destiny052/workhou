#ifndef IR_H
#define IR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    OP_ADD,
    OP_FADD,
    OP_SUB,
    OP_FSUB,
    OP_MUL,
    OP_FMUL,
    OP_DIV,
    OP_FDIV, 
    OP_MOD,
    OP_ASSIGN,
    OP_FASSIGN,
    OP_IF_FALSE,
    OP_GOTO,
    OP_LABEL,
    OP_FLOAT_TO_INT, 
    OP_INT_TO_FLOAT
};

// ===== 四元式结构 =====
typedef struct IRNode {
    int op;
    char arg1[32];
    char arg2[32];
    char result[32];
    struct IRNode* next;
} IRNode;

// ===== IR 链表 =====
typedef struct {
    IRNode* head;
    int use_count[1024];
} IRList;

// ===== 对外接口 =====
struct ASTNode;                 // 前向声明 AST
IRList* gen_ir(struct ASTNode* root);
void emit(IRList* ir, int op, const char* a1, const char* a2, const char* res);

#endif
