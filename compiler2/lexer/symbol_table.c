#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbol_table.h"



#define MAX_SYMBOLS 1000
#define MAX_NAME_LEN 64

typedef struct
{
    char name[MAX_NAME_LEN];
    int type;  // 数据类型：INT_TYPE, FLOAT_TYPE, DOUBLE_TYPE
    int size;  // 大小（字节）
} Symbol;

static Symbol table[MAX_SYMBOLS];
static int symbol_count = 0;


int lookup_symbol(const char *name)
{
    int i;
    for (i = 0; i < symbol_count; i++)
    {
        if (strcmp(table[i].name, name) == 0)
        {
            return i + 1; 
        }
    }
    return -1;
}


int insert_symbol(const char *name)
{
    int pos = lookup_symbol(name);
    if (pos != -1)
    {
        return pos; 
    }


    if (symbol_count < MAX_SYMBOLS)
    {
        strncpy(table[symbol_count].name, name, MAX_NAME_LEN - 1);
        table[symbol_count].name[MAX_NAME_LEN - 1] = '\0'; 
        symbol_count++;
        return symbol_count;
    }
    else
    {
        fprintf(stderr, "Symbol Table Overflow!\n");
        exit(1);
    }
}

int insert_symbol_with_type(const char *name, int type)
{
    int pos = lookup_symbol(name);
    if (pos != -1)
    {
        // 如果符号已存在，更新其类型
        fprintf(stderr, "[SymbolTable] Error: Redeclaration of '%s'\n", name);
        return -1;
    }

    // 插入新符号
    if (symbol_count < MAX_SYMBOLS)
    {
        strncpy(table[symbol_count].name, name, MAX_NAME_LEN - 1);
        table[symbol_count].name[MAX_NAME_LEN - 1] = '\0';
        table[symbol_count].type = type;  // 设置类型
        symbol_count++;
        return symbol_count;
    }
    else
    {
        fprintf(stderr, "[SymbolTable] Error: Symbol Table Overflow!\n");
        exit(1);
    }
}

int get_symbol_type(const char *name)
{
    int pos = lookup_symbol(name);
    if (pos != -1)
    {
        return table[pos-1].type;
    }
    return UNKNOWN_TYPE;  // 符号不存在
}
void print_symbol_table()
{
    int i;
    printf("\n=== SYMBOL TABLE ===\n");
    printf("Index | Name | Type\n");
    printf("-------------------\n");
    for (i = 0; i < symbol_count; i++)
    {
        const char* type_str = "unknown";
        switch (table[i].type)
        {
            case INT_TYPE: type_str = "int"; break;
            case FLOAT_TYPE: type_str = "float"; break;
            case DOUBLE_TYPE: type_str = "double"; break;
            case VOID_TYPE: type_str = "void"; break;
        }
        printf("%5d | %-10s | %s\n", i + 1, table[i].name, type_str);
    }
}
