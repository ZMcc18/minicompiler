# MiniCompiler

一个用于学习编译原理的简易编译器项目。

## 项目简介

MiniCompiler 是一个简易但功能完整的编译器实现，旨在帮助学习和理解现代编译器的工作原理。该项目借鉴了 LLVM、MLIR 和 TVM 等成熟编译器框架的设计理念，但规模更小，更易于理解。

本编译器支持一个类 C 语言的子集，实现了完整的编译流程，包括词法分析、语法分析、语义分析、中间表示生成、代码优化以及目标代码生成。

## 特性

- 完整的编译器前端、中端和后端实现
- 基于类 LLVM IR 的中间表示
- 多种代码优化技术
- 模块化设计，易于扩展
- 详细的文档和注释

## 项目结构

```
MiniCompiler/
├── include/              # 头文件
│   ├── lexer/            # 词法分析器
│   ├── parser/           # 语法分析器
│   ├── ast/              # 抽象语法树
│   ├── semantic/         # 语义分析
│   ├── ir/               # 中间表示
│   ├── optimizer/        # 优化器
│   └── codegen/          # 代码生成
├── src/                  # 源代码实现
├── examples/             # 示例代码
├── tests/                # 测试用例
├── docs/                 # 文档
├── CMakeLists.txt        # CMake 构建文件
└── README.md             # 项目说明
```

## 构建与安装

### 依赖项

- C++17 兼容的编译器 (GCC 7+, Clang 5+, MSVC 2019+)
- CMake 3.10+
- (可选) LLVM 10+

### 构建步骤

```bash
# 克隆仓库
git clone https://github.com/yourusername/MiniCompiler.git
cd MiniCompiler

# 创建构建目录
mkdir build && cd build

# 配置
cmake ..

# 构建
cmake --build .

# 运行测试
ctest
```

## 使用方法

```bash
# 编译源文件
./minicompiler input.mc -o output

# 查看生成的中间表示
./minicompiler input.mc --emit-ir

# 应用优化
./minicompiler input.mc -O1 -o output
```

## 示例

```c
// examples/hello.mc
int main() {
    int a = 10;
    int b = 20;
    int c = a + b;
    print(c);
    return 0;
}
```

编译并运行:

```bash
./minicompiler examples/hello.mc -o hello
./hello
# 输出: 30
```

## 学习资源

- [编译器设计文档](docs/design.md)
- [中间表示规范](docs/ir_spec.md)
- [优化器实现详解](docs/optimizations.md)

## 贡献

欢迎提交 Pull Request 或创建 Issue 来帮助改进这个项目。

## 许可证

MIT 