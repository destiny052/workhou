

// 1. 合法的声明
int valid_int = 10;
float valid_float = 3.14;
double valid_double = 2.71828;

// 2. 类型错误：float -> int
int error_int1 = valid_float;  // 错误

// 3. 类型错误：double -> int  
int error_int2 = valid_double;  // 错误

// 4. 允许的转换（但有警告）：double -> float
float warning_float = valid_double;  // 警告

// 5. 允许的转换：int -> float
float ok_float = valid_int;  // 允许

// 6. 允许的转换：int -> double
double ok_double = valid_int;  // 允许

// 7. 未声明的变量
unknown = 5;  // 错误

// 8. 重复声明
int valid_int = 20;  // 错误

// 9. 混合类型运算中的类型提升
float mixed1 = valid_float + valid_int;  // 允许：int提升为float
int mixed2 = valid_float + valid_int;    // 错误：结果float不能赋值给int

// 10. 赋值语句中的类型错误
int x;
x = valid_float;  // 错误

// 11. 表达式中的类型不匹配（如果支持函数）
// 假设有一个期望int参数的函数
// call_func(valid_float);  // 错误
