%{
#include <stdio.h>
#include <string.h>
#include "symbol_table.h"
#include "parser/mybison.tab.h"

int yylineno = 1;

void print_token(const char* token, const char* lexeme, int attr) {
    if (attr == -1)
        printf("LINE %d: %-10s %-12s\n", yylineno, token, lexeme);
    else
        printf("LINE %d: %-10s %-12s %d\n", yylineno, token, lexeme, attr);
}
%}

int yywrap(void) { return 1; }
%option nounput noinput
%option noyywrap

digit      [0-9]
id_start   [a-zA-Z_]
id_rest    [a-zA-Z0-9_]

%%

 /* 关键字 */
"int"        { print_token("INT", yytext, -1); return INT;}
"float"       { print_token("FLOAT", yytext, -1); return FLOAT;}
"double"      { print_token("DOUBLE", yytext, -1); return DOUBLE;}
"void"       { print_token("VOID", yytext, -1); return VOID;}
"if"         { print_token("IF", yytext, -1); return IF;}
"else"       { print_token("ELSE", yytext, -1); return ELSE;}
"while"      { print_token("WHILE", yytext, -1); return WHILE;}
"for"        { print_token("FOR", yytext, -1); return FOR;}
"return"     { print_token("RETURN", yytext, -1); return RETURN;}
"main"       { print_token("MAIN", yytext, -1); return MAIN;}

 /* 标识符 */
{id_start}{id_rest}* {
    print_token("ID", yytext, -1);
    yylval.str_val = strdup(yytext);
    return ID;
}

 /* 数字 */
{digit}+"."{digit}*([eE][+-]?{digit}+)? |
"."{digit}+([eE][+-]?{digit}+)? |
{digit}+[eE][+-]?{digit}+ {
    double val = atof(yytext);
    print_token("REAL_NUMBER", yytext, -1);
    yylval.double_val = val;  
    return REAL_NUMBER;
}
{digit}+ {
   int val = atoi(yytext);
    print_token("NUMBER", yytext, val);
    yylval.int_val = val;      
    return NUMBER;
}

 /* 关系运算符 */
"=="        { print_token("EQ", yytext, -1); return EQ;}
"!="        { print_token("NEQ", yytext, -1); return NEQ;}
">="        { print_token("GE", yytext, -1); return GE;}
"<="        { print_token("LE", yytext, -1); return LE;}
"="         { print_token("ASSIGN", yytext, -1); return ASSIGN;}
">"         { print_token("GT", yytext, -1); return GT;}
"<"         { print_token("LT", yytext, -1); return LT;}

 /* 逻辑运算符 */
"&&"        { print_token("AND", yytext, -1); return AND;}
"||"        { print_token("OR", yytext, -1); return OR;}
"!"         { print_token("NOT", yytext, -1); return NOT;}

 /* 算术运算符 */
"+"         { print_token("ADD", yytext, -1); return ADD;}
"-"         { print_token("SUB", yytext, -1); return SUB;}
"*"         { print_token("MUL", yytext, -1); return MUL;}
"/"         { print_token("DIV", yytext, -1); return DIV;}
"%"         { print_token("MOD", yytext, -1); return MOD;}

 /* 分隔符 */
";"         { print_token("SEMI", yytext, -1); return SEMI;}
","         { print_token("COMMA", yytext, -1); return COMMA;}
"("         { print_token("LP", yytext, -1); return LP;}
")"         { print_token("RP", yytext, -1); return RP;}
"{"         { print_token("LBRACE", yytext, -1); return LBRACE;}
"}"         { print_token("RBRACE", yytext, -1); return RBRACE;}

 /* 注释 */
"//".*      { /* ignore */ }
"/*"([^\*]|\*+[^/])*\*+"/"    { /* ignore */ }

 /* 空白字符 */
[ \t\r]+                  { /* skip */ }
\n                        { yylineno++; }

 /* 错误处理 */
. {
    printf("LEXICAL ERROR at line %d: Illegal character '%s'\n",
           yylineno, yytext);
}

%%