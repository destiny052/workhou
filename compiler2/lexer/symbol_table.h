#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H


int insert_symbol(const char* name);


int lookup_symbol(const char* name);


void print_symbol_table();


int insert_symbol_with_type(const char *name, int type);


int get_symbol_type(const char *name);

#define INT_TYPE 1
#define FLOAT_TYPE 2
#define DOUBLE_TYPE 3
#define VOID_TYPE 4
#define UNKNOWN_TYPE 0

#endif