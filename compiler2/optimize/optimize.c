#include "optimize.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int is_number(const char* s) {
    if (!s || !*s) return 0;
    if (*s == '-' || *s == '+') s++;
    while (*s) {
        if (*s < '0' || *s > '9') return 0;
        s++;
    }
    return 1;
}

typedef struct {
    char name[32];
    int value;
} ConstEntry;

static ConstEntry consts[256];
static int const_cnt = 0;

static int find_const(const char* name, int* out) {
    for (int i = 0; i < const_cnt; i++) {
        if (strcmp(consts[i].name, name) == 0) {
            *out = consts[i].value;
            return 1;
        }
    }
    return 0;
}

static void set_const(const char* name, int v) {
    for (int i = 0; i < const_cnt; i++) {
        if (strcmp(consts[i].name, name) == 0) {
            consts[i].value = v;
            return;
        }
    }
    strcpy(consts[const_cnt].name, name);
    consts[const_cnt].value = v;
    const_cnt++;
}


/* 操作码转字符串，方便输出 */
static const char* op_to_str(int op) {
    switch (op) {
case OP_ADD: return "ADD";
        case OP_FADD: return "FADD";
        case OP_SUB: return "SUB";
        case OP_FSUB: return "FSUB";
        case OP_MUL: return "MUL";
        case OP_FMUL: return "FMUL";
        case OP_DIV: return "DIV";
        case OP_FDIV: return "FDIV";
        case OP_MOD: return "MOD";
        case OP_ASSIGN: return "ASSIGN";
        case OP_FASSIGN: return "FASSIGN";
        case OP_IF_FALSE: return "IF_FALSE";
        case OP_GOTO: return "GOTO";
        case OP_LABEL: return "LABEL";
        case OP_FLOAT_TO_INT: return "FLOAT_TO_INT";
        case OP_INT_TO_FLOAT: return "INT_TO_FLOAT";
        default: return "UNKNOWN";
    }
}
static void invalidate_const(const char* name) {
    for (int i = 0; i < const_cnt; i++) {
        if (strcmp(consts[i].name, name) == 0) {
            // 标记为无效（如果你加了 valid 字段）
            return;
        }
    }
}

void optimize_ir(IRList* ir) {
    if (!ir || !ir->head) return;

    IRNode* p = ir->head;

    while (p) {
        if (p->op == OP_ADD || p->op == OP_SUB ||
            p->op == OP_MUL || p->op == OP_DIV) {

            int a, b;
            int ok1 = is_number(p->arg1) ? (a = atoi(p->arg1), 1)
                                         : find_const(p->arg1, &a);
            int ok2 = is_number(p->arg2) ? (b = atoi(p->arg2), 1)
                                         : find_const(p->arg2, &b);

            if (ok1 && ok2) {
                int r = 0;
                if (p->op == OP_ADD) r = a + b;
                if (p->op == OP_SUB) r = a - b;
                if (p->op == OP_MUL) r = a * b;
                if (p->op == OP_DIV && b != 0) r = a / b;

                p->op = OP_ASSIGN;
                sprintf(p->arg1, "%d", r);
                strcpy(p->arg2, "-");
                set_const(p->result, r);
            }
        }

        if (p->op == OP_ASSIGN) {
            if (is_number(p->arg1)) {
                set_const(p->result, atoi(p->arg1));
            } else {
                invalidate_const(p->result); // 建议加
            }
        }

        p = p->next;
    }
}
