// 计算阶乘的程序
int factorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

int main() {
    int n = 5;
    int result = factorial(n);
    print("Factorial of ");
    print(n);
    print(" is ");
    print(result);
    return 0;
} 