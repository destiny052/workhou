// 函数内联测试
int square(int x) {
    return x * x;
}

int cube(int x) {
    return x * x * x;
}

int main() {
    int a = 5;
    int b = square(a);  // 应该被内联
    int c = cube(a);    // 应该被内联
    
    // 常量传播后应该能进一步优化
    int d = b + c;      // 5*5 + 5*5*5 = 25 + 125 = 150
    
    return d;
}