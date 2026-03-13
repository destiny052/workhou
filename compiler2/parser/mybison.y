%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symbol_table.h"

int yylex();
void yyerror(const char *s);
extern int yylineno;
extern FILE *yyin;

/* 用于存储AST根节点 */
ASTNode *ast_root = NULL;

void print_ast_simple_info(ASTNode *node);

extern void add_symbol_type(const char* name, int type);
extern int get_symbol_type(const char* name);
%}


%code requires {
    struct ASTNode;
}

%union {
    int int_val;
    double double_val;
    char* str_val;
    struct ASTNode* node_val;  
}

%token INT VOID IF ELSE WHILE FOR RETURN MAIN
%token FLOAT DOUBLE
%token <double_val> REAL_NUMBER
%token <int_val> NUMBER
%token <str_val> ID

%token EQ NEQ GE LE GT LT
%token ASSIGN
%token AND OR NOT
%token ADD SUB MUL DIV MOD
%token SEMI COMMA LP RP LBRACE RBRACE

%type <node_val> program stmt_list stmt
%type <node_val> declaration decl_list init_declarator
%type <node_val> compound_stmt expr_stmt select_stmt iter_stmt jump_stmt
%type <node_val> expression assign_expr logic_or_expr logic_and_expr equality_expr
%type <node_val> relational_expr additive_expr multiplicative_expr unary_expr primary_expr
%type <str_val> type_specifier

%nonassoc ASSIGN
%left OR
%left AND
%left EQ NEQ
%left LT GT LE GE
%left ADD SUB
%left MUL DIV MOD
%right NOT

%%

/* 程序规则 */
program: MAIN LP RP LBRACE stmt_list RBRACE {
        ast_root = new_program_node($5);
        printf("\nParse successful\n");
        
        // 恢复树形输出
        printf("\n========== AST Tree ==========\n");
        print_ast(ast_root, 0);
        printf("=============================\n");
        
        // 简洁的AST信息输出
        //print_ast_simple_info(ast_root);
        
        print_symbol_table();
        $$ = ast_root;
    }
    | stmt_list {
        ast_root = new_program_node($1);
        printf("\nParse successful\n");
        
        // 恢复树形输出
        printf("\n========== AST Tree ==========\n");
        print_ast(ast_root, 0);
        printf("=============================\n");
        
        // 简洁的AST信息输出
        //print_ast_simple_info(ast_root);
        
        print_symbol_table();
        $$ = ast_root;
    }
    ;

stmt: declaration
    | compound_stmt
    | expr_stmt
    | select_stmt
    | iter_stmt
    | jump_stmt
    ;

stmt_list: stmt {
        $$ = $1;
    }
    | stmt_list stmt {
        /* 将新语句连接到链表末尾 */
        ASTNode *p = $1;
        if (!p) {
            $$ = $2;
        } else {
            while (p->next) p = p->next;
            p->next = $2;
            $$ = $1;
        }
    }
    ;
    /* 修改 stmt_list 规则，允许空 */
stmt_list: /* empty */ { $$ = NULL; }
    | stmt { $$ = $1; }
    | stmt_list stmt {
        ASTNode *p = $1;
        if (!p) {
            $$ = $2;
        } else {
            while (p->next) p = p->next;
            p->next = $2;
            $$ = $1;
        }
    }
    ;

type_specifier: INT { $$ = "int"; }
    | VOID { $$ = "void"; }
    | FLOAT { $$ = "float"; }    
    | DOUBLE { $$ = "double"; }
    ;

declaration: type_specifier decl_list SEMI {
        $$ = new_decl_node($1, $2);
        
        // ===== 添加：将类型信息存入符号表 =====
        // 获取类型字符串对应的类型常量
        int type_const;
        if (strcmp($1, "int") == 0) {
            type_const = INT_TYPE;
        } else if (strcmp($1, "float") == 0) {
            type_const = FLOAT_TYPE;
        } else if (strcmp($1, "double") == 0) {
            type_const = DOUBLE_TYPE;
        } else if (strcmp($1, "void") == 0) {
            type_const = VOID_TYPE;
        } else {
            type_const = UNKNOWN_TYPE;
        }
        
        // 遍历 decl_list 中的每个标识符
        ASTNode* p = $2;
        while (p) {
            ASTNode* id_node = p;
            
            // 处理赋值声明的情况（如 float f = 3.14;）
            if (p->type == AST_ASSIGN) {
                // AST_ASSIGN 节点的左子是标识符
                id_node = p->left;
            }
            
            // 检查是否是标识符节点
            if (id_node && id_node->type == AST_ID) {
                // 将标识符和类型存入符号表
                insert_symbol_with_type(id_node->value, type_const);
            }
            
            p = p->next;  // 移动到链表中的下一个声明
        }
        // ===== 添加结束 =====
    }
    ;

decl_list: init_declarator {
        $$ = $1;
    }
    | decl_list COMMA init_declarator {
        ASTNode *p = $1;
        if (!p) {
            $$ = $3;
        } else {
            while (p->next) p = p->next;
            p->next = $3;
            $$ = $1;
        }
    }
    ;

init_declarator: ID {
        $$ = new_id_leaf($1);
    }
    | ID ASSIGN expression {
        $$ = new_assign_node(new_id_leaf($1), $3);
    }
    ;

compound_stmt: LBRACE stmt_list RBRACE {
        $$ = new_compound_node($2);
    }
    | LBRACE RBRACE {  /* 处理空复合语句 */
        $$ = new_compound_node(NULL);
    }
    ;

expr_stmt: expression SEMI {
        $$ = $1;
    }
    | SEMI {
        $$ = NULL;
    }
    ;

select_stmt: IF LP expression RP stmt {
        $$ = new_if_node($3, $5, NULL);
    }
    | IF LP expression RP stmt ELSE stmt {
        $$ = new_if_node($3, $5, $7);
    }
    ;

iter_stmt: WHILE LP expression RP stmt {
        $$ = new_while_node($3, $5);
    }
    | FOR LP expr_stmt expr_stmt expression RP stmt {
        /* 修正for循环语法：for(init; cond; step) body */
        $$ = new_for_node($3, $5, $4, $7);
    }
    ;

jump_stmt: RETURN expression SEMI {
        $$ = new_return_node($2);
    }
    | RETURN SEMI {
        $$ = new_return_node(NULL);
    }
    ;

expression: assign_expr { $$ = $1; }
    ;

assign_expr: logic_or_expr { $$ = $1; }
    | ID ASSIGN assign_expr {
        $$ = new_assign_node(new_id_leaf($1), $3);
    }
    ;

logic_or_expr: logic_and_expr { $$ = $1; }
    | logic_or_expr OR logic_and_expr {
        $$ = new_binary_op_node("||", $1, $3);
    }
    ;

logic_and_expr: equality_expr { $$ = $1; }
    | logic_and_expr AND equality_expr {
        $$ = new_binary_op_node("&&", $1, $3);
    }
    ;

equality_expr: relational_expr { $$ = $1; }
    | equality_expr EQ relational_expr {
        $$ = new_binary_op_node("==", $1, $3);
    }
    | equality_expr NEQ relational_expr {
        $$ = new_binary_op_node("!=", $1, $3);
    }
    ;

relational_expr: additive_expr { $$ = $1; }
    | relational_expr LT additive_expr {
        $$ = new_binary_op_node("<", $1, $3);
    }
    | relational_expr GT additive_expr {
        $$ = new_binary_op_node(">", $1, $3);
    }
    | relational_expr LE additive_expr {
        $$ = new_binary_op_node("<=", $1, $3);
    }
    | relational_expr GE additive_expr {
        $$ = new_binary_op_node(">=", $1, $3);
    }
    ;

additive_expr: multiplicative_expr { $$ = $1; }
    | additive_expr ADD multiplicative_expr {
        $$ = new_binary_op_node("+", $1, $3);
    }
    | additive_expr SUB multiplicative_expr {
        $$ = new_binary_op_node("-", $1, $3);
    }
    ;

multiplicative_expr: unary_expr { $$ = $1; }
    | multiplicative_expr MUL unary_expr {
        $$ = new_binary_op_node("*", $1, $3);
    }
    | multiplicative_expr DIV unary_expr {
        $$ = new_binary_op_node("/", $1, $3);
    }
    | multiplicative_expr MOD unary_expr {
        $$ = new_binary_op_node("%", $1, $3);
    }
    ;

unary_expr: primary_expr { $$ = $1; }
    | ADD unary_expr %prec NOT { $$ = $2; }
    | SUB unary_expr %prec NOT {
        $$ = new_unary_op_node("NEG", $2);
    }
    | NOT unary_expr %prec NOT {
        $$ = new_unary_op_node("!", $2);
    }
    ;

primary_expr: ID {
        $$ = new_id_leaf($1);
    }
    | NUMBER {
        $$ = new_num_leaf($1);
    }
    | REAL_NUMBER { 
        $$ = new_real_leaf($1, DOUBLE_TYPE); 
    }
    | LP expression RP {
        $$ = $2;
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, s);
}
void print_ast_simple_info(ASTNode *node) {
    if (!node) {
        printf("AST is empty.\n");
        return;
    }
    
    printf("\nAST Summary:\n");
    printf("Root node type: ");
    
    switch (node->type) {
        case AST_PROGRAM: printf("PROGRAM\n"); break;
        case AST_BLOCK: printf("BLOCK\n"); break;
        case AST_DECL: printf("DECLARATION\n"); break;
        case AST_ASSIGN: printf("ASSIGNMENT\n"); break;
        case AST_BINOP: printf("BINARY OPERATION\n"); break;
        case AST_IF: printf("IF STATEMENT\n"); break;
        case AST_WHILE: printf("WHILE LOOP\n"); break;
        case AST_FOR: printf("FOR LOOP\n"); break;
        default: printf("OTHER\n"); break;
    }
}
