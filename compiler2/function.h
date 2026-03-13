#ifndef FUNCTION_H
#define FUNCTION_H

#include "ir/ir.h"

/* 函数信息结构 */
typedef struct {
    char* name;           /* 函数名 */
    char* return_type;    /* 返回类型 */
    IRList* ir_body;      /* 函数体IR */
    int param_count;      /* 参数个数 */
    char** param_types;   /* 参数类型数组 */
    char** param_names;   /* 参数名数组 */
    int is_inlineable;    /* 是否可内联 */
} FunctionInfo;

/* 函数表 */
typedef struct {
    FunctionInfo** functions;
    size_t capacity;
    size_t count;
} FunctionTable;

/* 函数声明 */
FunctionTable* func_table_create(size_t capacity);
void func_table_destroy(FunctionTable* table);
void func_table_add(FunctionTable* table, FunctionInfo* func);
FunctionInfo* func_table_get(FunctionTable* table, const char* name);
FunctionInfo* func_table_find_by_label(FunctionTable* table, const char* label);
int func_table_contains(FunctionTable* table, const char* name);

/* 函数信息操作 */
FunctionInfo* func_info_create(const char* name, const char* return_type);
void func_info_destroy(FunctionInfo* info);
void func_info_add_param(FunctionInfo* info, const char* type, const char* name);
void func_info_set_ir(FunctionInfo* info, IRList* ir);

/* 内联分析 */
int analyze_inlineability(FunctionInfo* info);

#endif