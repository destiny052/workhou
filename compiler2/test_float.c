// 浮点数和函数测试
float add(float x, float y) {
    return x + y;
}

int main() {
    float a = 3.14;
    float b = 2.71;
    float c = add(a, b);  // 函数调用
    
    // 常量折叠测试
    float d = 3.0 + 4.0 * 2.0;  // 应该被折叠为 d = 11.0
    
    // 混合类型运算
    int e = 5;
    float f = e + 1.5;    // 整型+浮点
    
    return (int)(c + d + f);
}