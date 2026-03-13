C语言子集编译器
这是一个用C语言实现的简单编译器，支持C语言的一个子集。它可以将源代码编译为中间代码（四元式）和MIPS汇编代码。该项目主要用于学习编译原理课程设计，展示了编译器的各个经典阶段：词法分析、语法分析、语义分析、中间代码生成、代码优化和目标代码生成。

功能特性
支持数据类型：int、float、double、void
支持关键字：int、float、double、void、if、else、while、for、return、main
支持算术运算：+、-、*、/、%、^（按位异或）
支持关系运算：==、!=、>、<、>=、<=
支持逻辑运算：&&、||、!
支持赋值运算：=
支持注释：// 和 /* */
完善的词法、语法和语义错误检测
生成三地址码（四元式）中间代码

基本代码优化（常量折叠、常量传播）
输出MIPS汇编代码
清晰的模块化设计，易于扩展

快速开始
环境要求
Linux / macOS / Windows（需要支持GCC、Flex、Bison）

在Ubuntu/Debian上安装依赖：
bash
sudo apt-get update
sudo apt-get install gcc flex bison make
编译
克隆项目后，进入项目根目录，运行：
bash
make
这将生成可执行文件 compiler。
运行
bash
./compiler < input.c
或者从文件读取：
bash
./compiler input.c
选项
-o <file>：指定输出文件名（默认为a.out）
-S：生成汇编代码（当前默认生成）
-O0：不进行优化
-O1：启用基本优化
-O2：启用更多优化（当前与O1相同）
-v：显示详细信息
-h：显示帮助信息

示例：
bash
./compiler -O1 -v test.c
测试
项目包含一些测试示例，位于tests/目录。运行测试：
bash
make test
或单独测试某个文件：
bash
./compiler tests/test1.c

项目结构
text
.
├── README.md
├── Makefile
├── lexer/
│   ├── lexer.l                # Flex词法规则
│   ├── symbol_table.c          # 符号表实现
│   └── symbol_table.h
├── parser/
│   ├── mybison.y               # Bison语法规则
│   ├── mybison.tab.c                   
│   └── mybison.tab.h
│   ├── mybison.tab.o                    
│   └── mybison.output
│   ├── ast.c                    # 抽象语法树实现
│   └── ast.h
├── ir/
│   ├── ir.c                     # 中间代码生成
│   ├── ir.h
├── optimize/
│   ├── optimize.c               # 代码优化
│   └── optimize.h
├── typecheck.c                  # 类型检查实现
└── typecheck.h
└── tests/
    ├── test1.c                  
    ├── test2.c             
    └── test3.c                 
    └── test4.c
    └── test5.c
 └── .h
 └── typecheck.h
 └── typecheck.h
编译流程说明
词法分析：使用Flex生成词法分析器，将源代码分割为Token序列。

语法分析：使用Bison生成语法分析器，根据语法规则构建抽象语法树（AST）。

语义分析：遍历AST进行类型检查，包括变量声明检查、类型兼容性检查等。

中间代码生成：将AST转换为三地址码（四元式）形式的中间表示。

代码优化：在中间代码上执行常量折叠等优化。

目标代码生成：将中间代码转换为MIPS汇编代码。

示例
输入（test.c）：

c
int main() {
    int a = 10;
    float b = 3.14;
    int sum = a + 20;
    if (a > 5) {
        b = b + 1.0;
    }
    return 0;
}
编译并查看中间代码：

bash
./compiler test.c
输出：

text
...
========== Intermediate Code (IR) ==========
OPCODE       ARG1       ARG2       RESULT
---------------------------------------------
ASSIGN       10         -          a
ASSIGN       3.140000   -          b
ADD          a          20         t1
ASSIGN       t1         -          sum
GT           a          5          t2
IF_FALSE     t2         -          L0
FADD         b          1.000000   t3
ASSIGN       t3         -          b
LABEL        -          -          L0
RETURN       0          -          -
---------------------------------------------
Total instructions: 10
=============================================

贡献指南
欢迎提交Issue和Pull Request。请确保代码风格一致，并添加适当的测试。

许可证
本项目采用MIT许可证。详情请参见LICENSE文件。
