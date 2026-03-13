#ifndef AST_H
#define AST_H

#include <stdio.h>

#define INT_TYPE 1
#define FLOAT_TYPE 2
#define DOUBLE_TYPE 3
#define VOID_TYPE 4

typedef enum {
    AST_BLOCK,
    AST_DECL,
    AST_ASSIGN,
    AST_BINOP,
    AST_NUM,
    AST_REAL,
    AST_FLOAT_NUM,
    AST_DOUBLE_NUM,
    AST_ID,
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_PROGRAM,
    AST_TYPE,
    AST_RETURN,
    AST_FUNC_DECL,
    AST_FUNC_PROTO,
    AST_FUNC_CALL,
    AST_PARAM,
    AST_TYPE_CAST,
    AST_UNARY_OP
} ASTType;

typedef struct ASTNode {
    ASTType type;
    char value[32];
    int data_type;
    union {
        int int_val;
        double double_val;
    } const_val; 

    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *cond;
    struct ASTNode *body;
    struct ASTNode *init;
    struct ASTNode *step;
    struct ASTNode *next;
    struct ASTNode *params;
    struct ASTNode *ret_type;
} ASTNode;

ASTNode *new_ast_node(ASTType type, const char *value,
                      ASTNode *left, ASTNode *right,
                      ASTNode *cond, ASTNode *body,
                      ASTNode *init, ASTNode *step,
                      ASTNode *next);
ASTNode *new_num_leaf(int val);
ASTNode *new_float_leaf(double val);
ASTNode *new_double_leaf(double val);
ASTNode *new_id_leaf(const char *name);
ASTNode *new_type_leaf(const char *type_name);
ASTNode *new_decl_node(const char *type, ASTNode *id_list);
ASTNode *new_assign_node(ASTNode *lhs, ASTNode *rhs);
ASTNode *new_if_node(ASTNode *cond, ASTNode *then_part, ASTNode *else_part);
ASTNode *new_while_node(ASTNode *cond, ASTNode *body);
ASTNode *new_for_node(ASTNode *init, ASTNode *cond, ASTNode *step, ASTNode *body);
ASTNode *new_compound_node(ASTNode *stmt_list);
ASTNode *new_return_node(ASTNode *expr);
ASTNode *new_binary_op_node(const char *op, ASTNode *left, ASTNode *right);
ASTNode *new_unary_op_node(const char *op, ASTNode *operand);
ASTNode *new_program_node(ASTNode *stmt_list);
ASTNode *new_func_decl_node(const char *ret_type, const char *name, 
                           ASTNode *params, ASTNode *body);
ASTNode *new_func_proto_node(const char *ret_type, const char *name,
                            ASTNode *params);
ASTNode *new_func_call_node(const char *name, ASTNode *args);
ASTNode *new_param_node(const char *type, const char *name);
ASTNode *new_real_leaf(double val, int type);
ASTNode *new_type_cast_node(const char* type_name, ASTNode* expr);
ASTNode* constant_folding(ASTNode* node);

void print_ast(ASTNode *node, int level);
void print_ast_to_file(ASTNode *node, FILE *file, int level);


#endif

