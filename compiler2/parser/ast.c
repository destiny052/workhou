#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include "ast.h"

/* 创建通用AST节点 */
ASTNode *new_ast_node(ASTType type, const char *value,
                      ASTNode *left, ASTNode *right,
                      ASTNode *cond, ASTNode *body,
                      ASTNode *init, ASTNode *step,
                      ASTNode *next) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for AST node\n");
        exit(1);
    }

    node->type = type;
    if (value) {
        strncpy(node->value, value, 31);
        node->value[31] = '\0';
    } else {
        node->value[0] = '\0';
    }
    
    node->const_val.double_val = 0.0;
    
    node->left = left;
    node->right = right;
    node->cond = cond;
    node->body = body;
    node->init = init;
    node->step = step;
    node->next = next;
    node->params = NULL;
    node->ret_type = NULL;

    return node;
}

/* 创建数字叶子节点 */
/* 修改 new_num_leaf 函数（如果还没修改） */
ASTNode* new_num_leaf(int val) {
    char val_str[32];
    sprintf(val_str, "%d", val);
    ASTNode* node = new_ast_node(AST_NUM, val_str, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    node->data_type = INT_TYPE;
    node->const_val.int_val = val;  /* 确保const_val成员存在 */
    return node;
}

/* 添加 new_real_leaf 函数（如果还没有） */
ASTNode* new_real_leaf(double val, int data_type) {
    char val_str[32];
    sprintf(val_str, "%f", val);
    ASTNode* node = new_ast_node(AST_REAL, val_str, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    node->data_type = data_type;
    node->const_val.double_val = val;  /* 确保const_val成员存在 */
    return node;
}

/* 创建浮点数叶子节点 */
ASTNode *new_float_leaf(double val) {
    ASTNode *node = new_ast_node(AST_FLOAT_NUM, "", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    node->const_val.double_val = val;
    snprintf(node->value, sizeof(node->value), "%.6f", val);
    return node;
}

/* 创建双精度叶子节点 */
ASTNode *new_double_leaf(double val) {
    ASTNode *node = new_ast_node(AST_DOUBLE_NUM, "", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    node->const_val.double_val = val; 
    snprintf(node->value, sizeof(node->value), "%.15f", val);
    return node;
}

/* 创建标识符叶子节点 */
ASTNode *new_id_leaf(const char *name) {
    return new_ast_node(AST_ID, name, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

/* 创建类型叶子节点 */
ASTNode *new_type_leaf(const char *type_name) {
    return new_ast_node(AST_TYPE, type_name, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

/* 创建声明节点 */
ASTNode *new_decl_node(const char *type, ASTNode *id_list) {
    ASTNode *type_node = new_type_leaf(type);
    return new_ast_node(AST_DECL, "", type_node, NULL, NULL, id_list, NULL, NULL, NULL);
}

/* 创建赋值节点 */
ASTNode *new_assign_node(ASTNode *lhs, ASTNode *rhs) {
    return new_ast_node(AST_ASSIGN, "=", lhs, rhs, NULL, NULL, NULL, NULL, NULL);
}

/* 创建if节点 */
ASTNode *new_if_node(ASTNode *cond, ASTNode *then_part, ASTNode *else_part) {
    return new_ast_node(AST_IF, "", cond, else_part, NULL, then_part, NULL, NULL, NULL);
}

/* 创建while节点 */
ASTNode *new_while_node(ASTNode *cond, ASTNode *body) {
    return new_ast_node(AST_WHILE, "", cond, NULL, NULL, body, NULL, NULL, NULL);
}

/* 创建for节点 */
ASTNode *new_for_node(ASTNode *init, ASTNode *cond, ASTNode *step, ASTNode *body) {
    return new_ast_node(AST_FOR, "", init, step, cond, body, NULL, NULL, NULL);
}

/* 创建复合语句节点 */
ASTNode *new_compound_node(ASTNode *stmt_list) {
    return new_ast_node(AST_BLOCK, "", stmt_list, NULL, NULL, NULL, NULL, NULL, NULL);
}

/* 创建return节点 */
ASTNode *new_return_node(ASTNode *expr) {
    return new_ast_node(AST_RETURN, "", expr, NULL, NULL, NULL, NULL, NULL, NULL);
}

/* 创建二元运算符节点 */
ASTNode *new_binary_op_node(const char *op, ASTNode *left, ASTNode *right) {
    return new_ast_node(AST_BINOP, op, left, right, NULL, NULL, NULL, NULL, NULL);
}

/* 创建一元运算符节点 */
ASTNode *new_unary_op_node(const char *op, ASTNode *operand) {
    return new_ast_node(AST_UNARY_OP, op, operand, NULL, NULL, NULL, NULL, NULL, NULL);
}

/* 创建程序节点 */
ASTNode *new_program_node(ASTNode *stmt_list) {
    return new_ast_node(AST_PROGRAM, "", stmt_list, NULL, NULL, NULL, NULL, NULL, NULL);
}

/* ==================== 新增函数相关节点 ==================== */

/* 创建函数声明节点 */
ASTNode *new_func_decl_node(const char *ret_type, const char *name, 
                           ASTNode *params, ASTNode *body) {
    ASTNode *node = new_ast_node(AST_FUNC_DECL, name, NULL, NULL, NULL, body, NULL, NULL, NULL);
    node->ret_type = new_type_leaf(ret_type);
    node->params = params;
    return node;
}

/* 创建函数原型节点 */
ASTNode *new_func_proto_node(const char *ret_type, const char *name,
                            ASTNode *params) {
    ASTNode *node = new_ast_node(AST_FUNC_PROTO, name, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    node->ret_type = new_type_leaf(ret_type);
    node->params = params;
    return node;
}

/* 创建函数调用节点 */
ASTNode *new_func_call_node(const char *name, ASTNode *args) {
    ASTNode *id_node = new_id_leaf(name);
    return new_ast_node(AST_FUNC_CALL, "", id_node, args, NULL, NULL, NULL, NULL, NULL);
}

/* 创建参数节点 */
ASTNode *new_param_node(const char *type, const char *name) {
    ASTNode *type_node = new_type_leaf(type);
    ASTNode *id_node = new_id_leaf(name);
    ASTNode *node = new_ast_node(AST_PARAM, "", type_node, id_node, NULL, NULL, NULL, NULL, NULL);
    return node;
}


ASTNode *new_type_cast_node(const char* type_name, ASTNode* expr)
{
    ASTNode *node = new_ast_node(AST_TYPE_CAST, type_name, expr, NULL, NULL, NULL, NULL, NULL, NULL);
    return node;
}



/* 打印AST树扩展 */
void print_ast(ASTNode *node, int level) {
    int i;
    if (!node)
        return;

    for (i = 0; i < level; i++)
        printf("  ");

    switch (node->type) {
    case AST_PROGRAM:
        printf("[PROGRAM]\n");
        break;
    case AST_BLOCK:
        printf("[{}]\n");
        break;
    case AST_DECL:
        printf("[DECL]\n");
        break;
    case AST_ASSIGN:
        printf("[=]\n");
        break;
    case AST_BINOP:
        printf("[%s]\n", node->value);
        break;
    case AST_UNARY_OP:
        printf("[%s]\n", node->value);
        break;
    case AST_NUM:
        printf("[NUM] %s\n", node->value);
        break;
    case AST_FLOAT_NUM:
        printf("[FLOAT] %s\n", node->value);
        break;
    case AST_DOUBLE_NUM:
        printf("[DOUBLE] %s\n", node->value);
        break;
    case AST_ID:
        printf("[ID] %s\n", node->value);
        break;
    case AST_TYPE:
        printf("[TYPE] '%s'\n", node->value);
        break;
    case AST_IF:
        printf("[IF]\n");
        break;
    case AST_WHILE:
        printf("[WHILE]\n");
        break;
    case AST_FOR:
        printf("[FOR]\n");
        break;
    case AST_RETURN:
        printf("[RETURN]\n");
        break;
    case AST_FUNC_DECL:
        printf("[FUNC_DECL] %s\n", node->value);
        break;
    case AST_FUNC_PROTO:
        printf("[FUNC_PROTO] %s\n", node->value);
        break;
    case AST_FUNC_CALL:
        printf("[FUNC_CALL]\n");
        break;
    case AST_PARAM:
        printf("[PARAM]\n");
        break;
    case AST_TYPE_CAST:
        printf("[TYPE_CAST] to %s\n", node->value);
        break;
    case AST_REAL:
        printf("[REAL] %s\n", node->value);
        break;
    default:
        printf("[UNKNOWN]\n");
        break;
    }

    if (node->left) print_ast(node->left, level + 1);
    if (node->right) print_ast(node->right, level + 1);
    if (node->cond) print_ast(node->cond, level + 1);
    if (node->body) print_ast(node->body, level + 1);
    if (node->init) print_ast(node->init, level + 1);
    if (node->step) print_ast(node->step, level + 1);
    if (node->params) print_ast(node->params, level + 1);
    if (node->ret_type) print_ast(node->ret_type, level + 1);
    if (node->next) print_ast(node->next, level);
}



/* ===== 常量折叠优化函数 ===== */

/* 辅助函数：判断节点是否为数值常量 */
static int is_constant(ASTNode* node) {
        return node &&
           (node->type == AST_NUM ||
            node->type == AST_REAL ||
            node->type == AST_FLOAT_NUM ||
            node->type == AST_DOUBLE_NUM);
}

/* 辅助函数：获取常量值 */
static double get_constant_value(ASTNode* node) {
    if (!node) return 0.0;

    if (node->type == AST_NUM) {
        return (double)node->const_val.int_val;
    }
    else if (node->type == AST_REAL) {
        return node->const_val.double_val;
    }
    return 0.0;
}

/* 辅助函数：获取常量类型 */
static int get_constant_type(ASTNode* node) {
    if (!node) return INT_TYPE;

    switch (node->type) {
        case AST_NUM:
            return INT_TYPE;
        case AST_FLOAT_NUM:
            return FLOAT_TYPE;
        case AST_DOUBLE_NUM:
        case AST_REAL:
            return DOUBLE_TYPE;
        default:
            return INT_TYPE;
    }
}


/* 辅助函数：创建新的常量节点 */
static ASTNode* create_constant_node(double value, int type) {
    if (type == INT_TYPE) {
        return new_num_leaf((int)value);
    }
    else if (type == FLOAT_TYPE || type == DOUBLE_TYPE) {
        return new_real_leaf(value, type);
    }
    return new_num_leaf((int)value);  /* 默认整数 */
}

/* 算术运算折叠 */
static double fold_arithmetic(const char* op, double left, double right) {
    if (strcmp(op, "+") == 0) return left + right;
    if (strcmp(op, "-") == 0) return left - right;
    if (strcmp(op, "*") == 0) return left * right;
    if (strcmp(op, "/") == 0) {
        if (right == 0.0) {
            fprintf(stderr, "Warning: Division by zero in constant folding\n");
            return left;  /* 避免除以零 */
        }
        return left / right;
    }
    if (strcmp(op, "%") == 0) {
        if (right == 0.0) {
            fprintf(stderr, "Warning: Modulo by zero in constant folding\n");
            return left;
        }
        return fmod(left, right);  /* 使用fmod处理浮点数取余 */
    }
    return 0.0;
}

/* 关系运算折叠 */
static int fold_relational(const char* op, double left, double right) {
    if (strcmp(op, "<") == 0) return left < right;
    if (strcmp(op, ">") == 0) return left > right;
    if (strcmp(op, "<=") == 0) return left <= right;
    if (strcmp(op, ">=") == 0) return left >= right;
    if (strcmp(op, "==") == 0) return fabs(left - right) < 1e-10;  /* 浮点数相等判断 */
    if (strcmp(op, "!=") == 0) return fabs(left - right) >= 1e-10;
    return 0;
}

/* 逻辑运算折叠 */
static int fold_logical(const char* op, double left, double right) {
    int left_bool = (left != 0.0);
    int right_bool = (right != 0.0);

    if (strcmp(op, "&&") == 0) return left_bool && right_bool;
    if (strcmp(op, "||") == 0) return left_bool || right_bool;
    return 0;
}

/* 一元运算折叠 */
static double fold_unary(const char* op, double operand) {
    if (strcmp(op, "+") == 0) return operand;
    if (strcmp(op, "-") == 0) return -operand;
    if (strcmp(op, "!") == 0) return (operand == 0.0) ? 1.0 : 0.0;
    return operand;
}

/* 类型提升规则（如果其他地方没有定义） */
static int get_promoted_type(int type1, int type2) {
    /* 类型提升规则：double > float > int */
    if (type1 == DOUBLE_TYPE || type2 == DOUBLE_TYPE) {
        return DOUBLE_TYPE;
    }
    if (type1 == FLOAT_TYPE || type2 == FLOAT_TYPE) {
        return FLOAT_TYPE;
    }
    return INT_TYPE;  /* 默认都是int */
}

/* 主常量折叠函数 */
ASTNode* constant_folding(ASTNode* node) {
    if (!node) return NULL;

    /* 先递归处理子节点 */
    node->left = constant_folding(node->left);
    node->right = constant_folding(node->right);
    node->cond = constant_folding(node->cond);
    node->body = constant_folding(node->body);
    node->init = constant_folding(node->init);
    node->step = constant_folding(node->step);
    node->next = constant_folding(node->next);

    /* 处理二元运算的常量折叠 */
    if (node->type == AST_BINOP) {
        ASTNode* left = node->left;
        ASTNode* right = node->right;

        /* 如果两个操作数都是常量，进行折叠 */
        if (is_constant(left) && is_constant(right)) {
            double left_val = get_constant_value(left);
            double right_val = get_constant_value(right);
            int left_type = get_constant_type(left);
            int right_type = get_constant_type(right);
            int result_type = get_promoted_type(left_type, right_type);

            double result_val;
            int is_bool_result = 0;

            /* 根据运算符类型进行不同的折叠 */
            if (strcmp(node->value, "==") == 0 || strcmp(node->value, "!=") == 0 ||
                strcmp(node->value, "<") == 0 || strcmp(node->value, ">") == 0 ||
                strcmp(node->value, "<=") == 0 || strcmp(node->value, ">=") == 0) {
                /* 关系运算：结果为布尔值（用整数表示） */
                result_val = fold_relational(node->value, left_val, right_val);
                result_type = INT_TYPE;  /* 布尔值作为整数 */
                is_bool_result = 1;
            }
            else if (strcmp(node->value, "&&") == 0 || strcmp(node->value, "||") == 0) {
                /* 逻辑运算：结果为布尔值 */
                result_val = fold_logical(node->value, left_val, right_val);
                result_type = INT_TYPE;  /* 布尔值作为整数 */
                is_bool_result = 1;
            }
            else {
                /* 算术运算 */
                result_val = fold_arithmetic(node->value, left_val, right_val);
            }

            /* 创建新的常量节点替换原节点 */
            printf("[Optimize] Constant folding: %f %s %f = %f\n",
                left_val, node->value, right_val, result_val);

            /* 注意：这里要释放原节点的内存，简化处理 */
            /* 实际应用中应该更仔细地处理内存释放 */

            if (is_bool_result) {
                return new_num_leaf((int)result_val);
            }
            else {
                return create_constant_node(result_val, result_type);
            }
        }

        /* 特殊优化：0 * x = 0, 1 * x = x, 0 + x = x */
        if (is_constant(left)) {
            double left_val = get_constant_value(left);

            if (strcmp(node->value, "*") == 0) {
                if (left_val == 0.0 || left_val == 0) {
                    printf("[Optimize] Simplify: 0 * expression = 0\n");
                    return create_constant_node(0.0, get_constant_type(left));
                }
                if (left_val == 1.0 || left_val == 1) {
                    printf("[Optimize] Simplify: 1 * expression = expression\n");
                    return right;  /* 返回右子树 */
                }
            }

            if (strcmp(node->value, "+") == 0) {
                if (left_val == 0.0 || left_val == 0) {
                    printf("[Optimize] Simplify: 0 + expression = expression\n");
                    return right;  /* 返回右子树 */
                }
            }
        }

        if (is_constant(right)) {
            double right_val = get_constant_value(right);

            if (strcmp(node->value, "*") == 0) {
                if (right_val == 0.0 || right_val == 0) {
                    printf("[Optimize] Simplify: expression * 0 = 0\n");
                    return create_constant_node(0.0, get_constant_type(right));
                }
                if (right_val == 1.0 || right_val == 1) {
                    printf("[Optimize] Simplify: expression * 1 = expression\n");
                    return left;  /* 返回左子树 */
                }
            }

            if (strcmp(node->value, "+") == 0) {
                if (right_val == 0.0 || right_val == 0) {
                    printf("[Optimize] Simplify: expression + 0 = expression\n");
                    return left;  /* 返回左子树 */
                }
            }
        }
    }

    /* 处理一元运算的常量折叠 */
    if (node->type == AST_UNARY_OP && node->left) {
        /* 一元运算：如 -x, !x, +x */
        if (is_constant(node->left)) {
            double operand_val = get_constant_value(node->left);
            double result_val = fold_unary(node->value, operand_val);

            printf("[Optimize] Constant folding: %s %f = %f\n",
                node->value, operand_val, result_val);

            return create_constant_node(result_val, get_constant_type(node->left));
        }
    }

    return node;
}

/* 打印AST到文件扩展 */
void print_ast_to_file(ASTNode *node, FILE *file, int level) {
    int i;
    if (!node)
        return;

    for (i = 0; i < level; i++)
        fprintf(file, "  ");

    switch (node->type) {
    case AST_PROGRAM:
        fprintf(file, "[PROGRAM]\n");
        break;
    case AST_BLOCK:
        fprintf(file, "[{}]\n");
        break;
    case AST_DECL:
        fprintf(file, "[DECL]\n");
        break;
    case AST_ASSIGN:
        fprintf(file, "[=]\n");
        break;
    case AST_BINOP:
        fprintf(file, "[%s]\n", node->value);
        break;
    case AST_UNARY_OP:
        fprintf(file, "[%s]\n", node->value);
        break;
    case AST_NUM:
        fprintf(file, "[NUM] %s\n", node->value);
        break;
    case AST_FLOAT_NUM:
        fprintf(file, "[FLOAT] %s\n", node->value);
        break;
    case AST_DOUBLE_NUM:
        fprintf(file, "[DOUBLE] %s\n", node->value);
        break;
    case AST_ID:
        fprintf(file, "[ID] %s\n", node->value);
        break;
    case AST_TYPE:
        fprintf(file, "[TYPE] '%s'\n", node->value);
        break;
    case AST_IF:
        fprintf(file, "[IF]\n");
        break;
    case AST_WHILE:
        fprintf(file, "[WHILE]\n");
        break;
    case AST_FOR:
        fprintf(file, "[FOR]\n");
        break;
    case AST_RETURN:
        fprintf(file, "[RETURN]\n");
        break;
    case AST_FUNC_DECL:
        fprintf(file, "[FUNC_DECL] %s\n", node->value);
        break;
    case AST_FUNC_PROTO:
        fprintf(file, "[FUNC_PROTO] %s\n", node->value);
        break;
    case AST_FUNC_CALL:
        fprintf(file, "[FUNC_CALL]\n");
        break;
    case AST_PARAM:
        fprintf(file, "[PARAM]\n");
        break;
    case AST_TYPE_CAST:
        fprintf(file, "[TYPE_CAST] to %s\n", node->value);
        break;
    case AST_REAL:
        fprintf(file, "[REAL] %s\n", node->value);
        break;
    default:
        fprintf(file, "[UNKNOWN]\n");
        break;
    }

    print_ast_to_file(node->left, file, level + 1);
    print_ast_to_file(node->right, file, level + 1);
    print_ast_to_file(node->cond, file, level + 1);
    print_ast_to_file(node->body, file, level + 1);
    print_ast_to_file(node->init, file, level + 1);
    print_ast_to_file(node->step, file, level + 1);
    print_ast_to_file(node->params, file, level + 1);
    print_ast_to_file(node->ret_type, file, level + 1);
    print_ast_to_file(node->next, file, level);
}
