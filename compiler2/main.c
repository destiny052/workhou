#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;
extern int yyparse(void);
extern struct ASTNode *ast_root;

#include "parser/ast.h"    
#include "ir/ir.h"
#include "typecheck.h"
#include "optimize/optimize.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: compiler <source.c>\n");
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin)
    {
        perror("open source file failed");
        return 1;
    }

    printf("Starting syntax analysis...\n");
    if (yyparse() != 0)
    {
        printf("Parse failed.\n");
        fclose(yyin);
        return 1;
    }

    if (!ast_root)
    {
        printf("Error: AST root is NULL\n");
        fclose(yyin);
        return 1;
    }

    printf("Parse success.\n");



    /* ================= AST 类型检查 ================= */
    printf("Type checking (AST)...\n");
    /* ================= AST 类型检查 ================= */
    if (!typecheck_ast(ast_root)) {
    	printf("\nType check failed. Compilation aborted.\n");
    	fclose(yyin);
    	return 1;  // 停止编译
}

    /* ================= AST 常量折叠 ================= */
    printf("AST constant folding...\n");
    ast_root = constant_folding(ast_root);

    /* ================= IR 生成 ================= */
    printf("Generating IR...\n");
    IRList *ir = gen_ir(ast_root);
    if (!ir)
    {
        printf("IR generation failed.\n");
        fclose(yyin);
        return 1;
    }

    /* ================= IR 类型检查 ================= */
    printf("Type checking (IR)...\n");
    typecheck_ir(ir);

    /* ================= IR 优化（常量传播） ================= */
    printf("Optimizing IR...\n");
    optimize_ir(ir);

    fclose(yyin);
    printf("Compilation completed.\n");
    return 0;
}
