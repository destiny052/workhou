CC = gcc
CFLAGS = -g -Wall -std=c99 -I. -Iir -Ioptimize -Iparser -Ilexer

TARGET = compiler.exe

# ========== 对象文件列表 ==========
OBJS = main.o \
       ir/ir.o \
       optimize/optimize.o \
       parser/ast.o \
       parser/mybison.tab.o \
       lexer/lex.yy.o \
       lexer/symbol_table.o \
       lexer/wrapper.o \
       typecheck.o

# ========== 所有目标 ==========
all: $(TARGET)

# ========== 链接目标可执行文件 ==========
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# ========== 编译规则 ==========

# 主程序
main.o: main.c typecheck.h ir/ir.h optimize/optimize.h
	$(CC) $(CFLAGS) -c main.c -o main.o

# IR模块
ir/ir.o: ir/ir.c ir/ir.h parser/ast.h
	$(CC) $(CFLAGS) -c ir/ir.c -o ir/ir.o

# 优化模块
optimize/optimize.o: optimize/optimize.c optimize/optimize.h ir/ir.h
	$(CC) $(CFLAGS) -c optimize/optimize.c -o optimize/optimize.o

# AST模块
parser/ast.o: parser/ast.c parser/ast.h
	$(CC) $(CFLAGS) -c parser/ast.c -o parser/ast.o

# Bison生成的解析器
parser/mybison.tab.o: parser/mybison.tab.c parser/mybison.tab.h parser/ast.h lexer/symbol_table.h
	$(CC) $(CFLAGS) -c parser/mybison.tab.c -o parser/mybison.tab.o

# 类型检查模块
typecheck.o: typecheck.c typecheck.h parser/ast.h lexer/symbol_table.h
	$(CC) $(CFLAGS) -c typecheck.c -o typecheck.o

# Flex生成的词法分析器
lexer/lex.yy.o: lexer/lex.yy.c parser/mybison.tab.h lexer/symbol_table.h
	$(CC) $(CFLAGS) -c lexer/lex.yy.c -o lexer/lex.yy.o

# 符号表模块
lexer/symbol_table.o: lexer/symbol_table.c lexer/symbol_table.h
	$(CC) $(CFLAGS) -c lexer/symbol_table.c -o lexer/symbol_table.o

# Windows包装器
lexer/wrapper.o: lexer/wrapper.c
	$(CC) $(CFLAGS) -c lexer/wrapper.c -o lexer/wrapper.o

# ========== 生成规则 ==========

# Bison: 从 .y 文件生成 .tab.c 和 .tab.h
parser/mybison.tab.c parser/mybison.tab.h: parser/mybison.y
	cd parser && bison -d mybison.y -Wcounterexamples

# Flex: 从 .l 文件生成 .yy.c
lexer/lex.yy.c: lexer/lexer.l parser/mybison.tab.h
	cd lexer && flex lexer.l

# ========== 测试规则 ==========

# 测试基础表达式
test1: $(TARGET)
	./$(TARGET) test/test1.c
	@echo "=== Test 1 Complete ==="

# 测试实型数据
test_real: $(TARGET)
	./$(TARGET) test/test_real.c
	@echo "=== Real Data Test Complete ==="

# 测试类型检查
test_type: $(TARGET)
	./$(TARGET) test/test_type.c
	@echo "=== Type Check Test Complete ==="

# 运行所有测试
test: test1 test_real test_type
	@echo "=== All Tests Complete ==="

# ========== 清理规则 ==========

clean:
	del /Q *.o $(TARGET) 2>nul
	del /Q ir\*.o 2>nul
	del /Q optimize\*.o 2>nul
	del /Q parser\*.o 2>nul
	del /Q lexer\*.o 2>nul
	del /Q parser\mybison.tab.* parser\mybison.output 2>nul
	del /Q lexer\lex.yy.c 2>nul
	@echo "Clean complete."

# Linux/MacOS 兼容的清理规则（如果你也在这些系统上使用）
clean-unix:
	rm -f *.o $(TARGET)
	rm -f ir/*.o
	rm -f optimize/*.o
	rm -f parser/*.o
	rm -f lexer/*.o
	rm -f parser/mybison.tab.* parser/mybison.output
	rm -f lexer/lex.yy.c
	@echo "Clean complete."

# ========== 依赖关系 ==========

# 依赖关系（可选，用于自动生成依赖）
depend:
	$(CC) -MM $(CFLAGS) main.c > .depend
	$(CC) -MM $(CFLAGS) ir/ir.c >> .depend
	$(CC) -MM $(CFLAGS) optimize/optimize.c >> .depend
	$(CC) -MM $(CFLAGS) parser/ast.c >> .depend
	$(CC) -MM $(CFLAGS) typecheck.c >> .depend
	$(CC) -MM $(CFLAGS) lexer/symbol_table.c >> .depend

# 包含依赖文件
-include .depend

# ========== 辅助规则 ==========

# 重新生成所有（完全重新编译）
rebuild: clean all

# 快速编译（不重新生成bison/flex文件）
quick: $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# 查看编译器版本信息
info:
	@echo "Compiler: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "Target: $(TARGET)"
	@echo "Source files:"
	@echo "  $(OBJS:.o=.c)"

.PHONY: all clean clean-unix test test1 test_real test_type rebuild quick info depend