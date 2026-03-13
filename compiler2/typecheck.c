#include "typecheck.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser/ast.h"
#include "lexer/symbol_table.h"  

const char* type_to_string(int type) {
    switch (type) {
        case INT_TYPE: return "int";
        case FLOAT_TYPE: return "float";
        case DOUBLE_TYPE: return "double";
        case VOID_TYPE: return "void";
        default: return "unknown";
    }
}


void set_node_type(ASTNode* node, int type) {
    if (node) {
        node->data_type = type;
    }
}


int get_expr_type(ASTNode* node) {
    if (!node) return UNKNOWN_TYPE;

    switch (node->type) {
    case AST_NUM:
        return INT_TYPE;
    case AST_REAL:
        return node->data_type;
    case AST_ID: {
        // 从符号表获取标识符的类型
        int type = get_symbol_type(node->value);
        if (type == UNKNOWN_TYPE) {  // 注意：UNKNOWN_TYPE应该定义为-1或其他值
            fprintf(stderr, "[TypeCheck] Error: Undefined variable '%s'\n", node->value);
            return UNKNOWN_TYPE;
        }
        return type;
    }
        case AST_BINOP:
            return node->data_type; 
        case AST_UNARY_OP:
            return node->data_type;  
        case AST_ASSIGN:
            return node->data_type; 
        default:
            return UNKNOWN_TYPE;
    }
}


int get_promoted_type(int type1, int type2) {
  
    if (type1 == DOUBLE_TYPE || type2 == DOUBLE_TYPE) {
        return DOUBLE_TYPE;
    }
    if (type1 == FLOAT_TYPE || type2 == FLOAT_TYPE) {
        return FLOAT_TYPE;
    }
    return INT_TYPE;
}


int can_convert(int from_type, int to_type) {
    // 所有类型都可以转换为自身
    if (from_type == to_type) return 1;

    // 允许的转换：
    // int -> float
    // int -> double
    // float -> double
    // double -> float (允许，但可能有精度损失)
    if (from_type == INT_TYPE && (to_type == FLOAT_TYPE || to_type == DOUBLE_TYPE)) {
        return 1;
    }
    if (from_type == FLOAT_TYPE && to_type == DOUBLE_TYPE) {
        return 1;
    }
    if (from_type == DOUBLE_TYPE && to_type == FLOAT_TYPE) {
        // 允许 double -> float 转换，但可能有精度损失
        return 1;
    }

    return 0;
}


int check_binary_op_types(int left_type, int right_type, const char* op) {
   
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || 
        strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || 
        strcmp(op, "%") == 0) {
        
       
        if ((left_type == INT_TYPE || left_type == FLOAT_TYPE || left_type == DOUBLE_TYPE) &&
            (right_type == INT_TYPE || right_type == FLOAT_TYPE || right_type == DOUBLE_TYPE)) {
            return 1;
        }
        
        fprintf(stderr, "[TypeCheck] Error: Operator '%s' requires numeric operands\n", op);
        return 0;
    }
    
  
    if (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
        
        if ((left_type == INT_TYPE || left_type == FLOAT_TYPE || left_type == DOUBLE_TYPE) &&
            (right_type == INT_TYPE || right_type == FLOAT_TYPE || right_type == DOUBLE_TYPE)) {
            return 1;
        }
        fprintf(stderr, "[TypeCheck] Error: Logical operator '%s' requires boolean-compatible operands\n", op);
        return 0;
    }
    
    return 1;
}

int check_relational_op_types(int left_type, int right_type) {
   
    if ((left_type == INT_TYPE || left_type == FLOAT_TYPE || left_type == DOUBLE_TYPE) &&
        (right_type == INT_TYPE || right_type == FLOAT_TYPE || right_type == DOUBLE_TYPE)) {
        return 1;
    }
    
    fprintf(stderr, "[TypeCheck] Error: Relational operators require numeric operands\n");
    return 0;
}


int check_equality_op_types(int left_type, int right_type) {
    
    if (left_type == right_type) {
        return 1; 
    }
    
   
    if ((left_type == INT_TYPE && (right_type == FLOAT_TYPE || right_type == DOUBLE_TYPE)) ||
        (right_type == INT_TYPE && (left_type == FLOAT_TYPE || left_type == DOUBLE_TYPE))) {
        return 1;
    }
    
 
    if ((left_type == FLOAT_TYPE && right_type == DOUBLE_TYPE) ||
        (right_type == FLOAT_TYPE && left_type == DOUBLE_TYPE)) {
        return 1;
    }
    
    fprintf(stderr, "[TypeCheck] Error: Equality operators require compatible types\n");
    return 0;
}


int check_assign_types(int lhs_type, int rhs_type) {
    // 1. 相同类型总是允许
    if (lhs_type == UNKNOWN_TYPE || rhs_type == UNKNOWN_TYPE) {
        return 0;
    }
    if (lhs_type == rhs_type) {
        return 1;
    }

    // 2. 特定不允许的转换（float/double -> int）
    if ((rhs_type == FLOAT_TYPE || rhs_type == DOUBLE_TYPE) && lhs_type == INT_TYPE) {
        fprintf(stderr, "[TypeCheck] Error: Cannot convert floating-point value to int without explicit cast\n");
        return 0;
    }

    // 3. 检查其他可转换的类型
    if (can_convert(rhs_type, lhs_type)) {
        // 如果是double到float的转换，给出警告
        if (rhs_type == DOUBLE_TYPE && lhs_type == FLOAT_TYPE) {
            printf("[TypeCheck] Warning: Implicit conversion from double to float may lose precision\n");
        }
        // 如果是int到float/double的转换，也可以给个提示
        else if (rhs_type == INT_TYPE && (lhs_type == FLOAT_TYPE || lhs_type == DOUBLE_TYPE)) {
            printf("[TypeCheck] Note: Implicit conversion from int to %s\n",
                type_to_string(lhs_type));
        }
        return 1;
    }

    // 4. 其他所有不允许的转换
    fprintf(stderr, "[TypeCheck] Error: Cannot assign %s to %s\n",
        type_to_string(rhs_type), type_to_string(lhs_type));
    return 0;
}

        
static int check_ast_node(ASTNode* node) {
    if (!node) return 1;

    // ===== 1. 先递归检查子节点（后序遍历）=====
    int ok = 1;
    ok &= check_ast_node(node->left);
    ok &= check_ast_node(node->right);
    ok &= check_ast_node(node->cond);
    ok &= check_ast_node(node->body);
    ok &= check_ast_node(node->init);
    ok &= check_ast_node(node->step);
    ok &= check_ast_node(node->next);

    if (!ok) return 0;

    // ===== 2. 再处理当前节点 =====
    switch (node->type) {

        case AST_ASSIGN: {
            int lhs_type = get_expr_type(node->left);
            int rhs_type = get_expr_type(node->right);

    	    if (lhs_type == UNKNOWN_TYPE || rhs_type == UNKNOWN_TYPE) {
            	return 0;
   	    }
            if (!check_assign_types(lhs_type, rhs_type)) {
                return 0;
            }

            set_node_type(node, lhs_type);
            return 1;
        }

        case AST_BINOP: {
            int left_type = get_expr_type(node->left);
            int right_type = get_expr_type(node->right);

            if (left_type == UNKNOWN_TYPE || right_type == UNKNOWN_TYPE)
                return 0;

            if (strcmp(node->value, "==") == 0 ||
                strcmp(node->value, "!=") == 0) {

                if (!check_equality_op_types(left_type, right_type))
                    return 0;

                set_node_type(node, INT_TYPE);
                return 1;
            }

            if (strcmp(node->value, "<") == 0 || strcmp(node->value, ">") == 0 ||
                strcmp(node->value, "<=") == 0 || strcmp(node->value, ">=") == 0) {

                if (!check_relational_op_types(left_type, right_type))
                    return 0;

                set_node_type(node, INT_TYPE);
                return 1;
            }

            if (!check_binary_op_types(left_type, right_type, node->value))
                return 0;

            int result_type = get_promoted_type(left_type, right_type);
            set_node_type(node, result_type);
            return 1;
        }

        default:
            return 1;
    }
}


/* 对 AST 进行类型检查 */
int typecheck_ast(ASTNode* node) {
    if (!node) {
        printf("[TypeCheck] AST is NULL\n");
        return;
    }
    
    printf("[TypeCheck] Starting AST type check...\n");
    
    if (check_ast_node(node)) {
        printf("[TypeCheck] AST type check passed.\n");
	return 1; 
    } else {
        fprintf(stderr, "[TypeCheck] AST type check failed.\n");
	return 0;
        // 可以根据需要决定是否继续编译
    }
}

/* 对 IR 进行类型检查（扩展版） */
void typecheck_ir(IRList* ir) {
    if (!ir || !ir->head) {
        printf("[TypeCheck] Empty IR.\n");
        return;
    }

    printf("[TypeCheck] Starting IR type check...\n");
    
    IRNode* p = ir->head;
    int ir_count = 0;
    int error_count = 0;
    
    while (p) {
        ir_count++;
        
        // 这里可以添加IR级别的类型检查
        // 例如：检查浮点运算指令的操作数类型
        // 检查赋值指令的左右类型是否匹配等
        
        p = p->next;
    }
    
    if (error_count == 0) {
        printf("[TypeCheck] IR type check finished. Checked %d IR instructions.\n", ir_count);
    } else {
        printf("[TypeCheck] IR type check finished with %d errors.\n", error_count);
    }
}
