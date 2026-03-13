#include "function.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 创建函数表 */
FunctionTable* func_table_create(size_t capacity) {
    FunctionTable* table = malloc(sizeof(FunctionTable));
    if (!table) return NULL;
    
    table->functions = calloc(capacity, sizeof(FunctionInfo*));
    if (!table->functions) {
        free(table);
        return NULL;
    }
    
    table->capacity = capacity;
    table->count = 0;
    return table;
}

/* 销毁函数表 */
void func_table_destroy(FunctionTable* table) {
    if (!table) return;
    
    for (size_t i = 0; i < table->count; i++) {
        if (table->functions[i]) {
            func_info_destroy(table->functions[i]);
        }
    }
    
    free(table->functions);
    free(table);
}

/* 添加函数 */
void func_table_add(FunctionTable* table, FunctionInfo* func) {
    if (!table || !func || table->count >= table->capacity) return;
    
    table->functions[table->count++] = func;
}

/* 获取函数 */
FunctionInfo* func_table_get(FunctionTable* table, const char* name) {
    if (!table || !name) return NULL;
    
    for (size_t i = 0; i < table->count; i++) {
        if (table->functions[i] && 
            strcmp(table->functions[i]->name, name) == 0) {
            return table->functions[i];
        }
    }
    
    return NULL;
}

/* 通过标签查找函数 */
FunctionInfo* func_table_find_by_label(FunctionTable* table, const char* label) {
    if (!table || !label) return NULL;
    
    for (size_t i = 0; i < table->count; i++) {
        if (table->functions[i] && 
            table->functions[i]->ir_body &&
            table->functions[i]->ir_body->func_name &&
            strstr(label, table->functions[i]->ir_body->func_name) != NULL) {
            return table->functions[i];
        }
    }
    
    return NULL;
}

/* 检查函数是否存在 */
int func_table_contains(FunctionTable* table, const char* name) {
    return func_table_get(table, name) != NULL;
}

/* 创建函数信息 */
FunctionInfo* func_info_create(const char* name, const char* return_type) {
    FunctionInfo* info = malloc(sizeof(FunctionInfo));
    if (!info) return NULL;
    
    info->name = strdup(name);
    info->return_type = strdup(return_type);
    info->ir_body = NULL;
    info->param_count = 0;
    info->param_types = NULL;
    info->param_names = NULL;
    info->is_inlineable = 0;
    
    return info;
}

/* 销毁函数信息 */
void func_info_destroy(FunctionInfo* info) {
    if (!info) return;
    
    free(info->name);
    free(info->return_type);
    
    if (info->param_types) {
        for (int i = 0; i < info->param_count; i++) {
            free(info->param_types[i]);
            free(info->param_names[i]);
        }
        free(info->param_types);
        free(info->param_names);
    }
    
    free(info);
}

/* 添加参数 */
void func_info_add_param(FunctionInfo* info, const char* type, const char* name) {
    if (!info || !type || !name) return;
    
    int new_count = info->param_count + 1;
    char** new_types = realloc(info->param_types, new_count * sizeof(char*));
    char** new_names = realloc(info->param_names, new_count * sizeof(char*));
    
    if (!new_types || !new_names) return;
    
    new_types[info->param_count] = strdup(type);
    new_names[info->param_count] = strdup(name);
    
    info->param_types = new_types;
    info->param_names = new_names;
    info->param_count = new_count;
}

/* 设置IR */
void func_info_set_ir(FunctionInfo* info, IRList* ir) {
    if (!info) return;
    
    info->ir_body = ir;
    if (ir && ir->func_name) {
        free(info->name);
        info->name = strdup(ir->func_name);
    }
    
    /* 分析是否可内联 */
    info->is_inlineable = analyze_inlineability(info);
}

/* 分析内联可能性 */
int analyze_inlineability(FunctionInfo* info) {
    if (!info || !info->ir_body) return 0;
    
    int instruction_count = 0;
    int has_call = 0;
    int has_loop = 0;
    
    IRNode* node = info->ir_body->head;
    while (node) {
        instruction_count++;
        
        /* 检查是否有函数调用 */
        if (node->op == OP_CALL) {
            has_call = 1;
        }
        
        /* 检查是否有循环 */
        if (node->op == OP_GOTO || node->op == OP_IF_FALSE) {
            has_loop = 1;
        }
        
        node = node->next;
    }
    
    /* 内联条件：
       1. 指令数较少（小于20条）
       2. 没有调用其他函数
       3. 没有复杂循环
    */
    return (instruction_count < 20 && !has_call && !has_loop);
}