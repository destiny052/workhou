#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "ir/ir.h"
#include "parser/ast.h"  // 添加AST头文件，以便直接操作AST节点

// ========== 类型常量定义 ==========
#define INT_TYPE 1
#define FLOAT_TYPE 2
#define DOUBLE_TYPE 3
#define VOID_TYPE 4
#define UNKNOWN_TYPE -1

// ========== 类型检查函数声明 ==========
void typecheck_ir(IRList *ir);       // 对 IR 进行类型检查
int typecheck_ast(ASTNode *node);   // 对 AST 进行类型检查

// ========== 类型兼容性检查函数 ==========
int check_binary_op_types(int left_type, int right_type, const char* op);
int check_assign_types(int lhs_type, int rhs_type);
int check_relational_op_types(int left_type, int right_type);
int check_equality_op_types(int left_type, int right_type);

// ========== 类型转换和提升 ==========
int get_promoted_type(int type1, int type2);  // 类型提升规则
int can_convert(int from_type, int to_type);  // 检查类型转换是否合法

// ========== 辅助函数 ==========
const char* type_to_string(int type);
void set_node_type(ASTNode* node, int type);  // 设置AST节点类型

#endif