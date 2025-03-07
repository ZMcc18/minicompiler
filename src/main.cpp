#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ir/ir_builder.h"
#include "optimizer/optimizer.h"
#include "codegen/code_generator.h"

using namespace minicompiler;

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " [options] <input_file>" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -o <output_file>   Specify output file (default: a.out)" << std::endl;
    std::cerr << "  --emit-ir          Output LLVM IR instead of executable" << std::endl;
    std::cerr << "  -O0                No optimizations" << std::endl;
    std::cerr << "  -O1                Basic optimizations" << std::endl;
    std::cerr << "  -O2                More aggressive optimizations" << std::endl;
    std::cerr << "  -h, --help         Display this help message" << std::endl;
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open file '" << filename << "' for writing" << std::endl;
        return false;
    }
    
    file << content;
    return true;
}

int main(int argc, char* argv[]) {
    // 解析命令行参数
    std::string inputFile;
    std::string outputFile = "a.out";
    bool emitIR = false;
    int optimizationLevel = 0;
    
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: -o option requires an argument" << std::endl;
                return 1;
            }
        } else if (strcmp(argv[i], "--emit-ir") == 0) {
            emitIR = true;
        } else if (strcmp(argv[i], "-O0") == 0) {
            optimizationLevel = 0;
        } else if (strcmp(argv[i], "-O1") == 0) {
            optimizationLevel = 1;
        } else if (strcmp(argv[i], "-O2") == 0) {
            optimizationLevel = 2;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        } else if (argv[i][0] == '-') {
            std::cerr << "Error: Unknown option '" << argv[i] << "'" << std::endl;
            printUsage(argv[0]);
            return 1;
        } else {
            inputFile = argv[i];
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    // 读取源文件
    std::string source = readFile(inputFile);
    if (source.empty()) {
        return 1;
    }
    
    try {
        // 词法分析
        std::cout << "Lexical analysis..." << std::endl;
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();
        
        // 语法分析
        std::cout << "Syntax analysis..." << std::endl;
        Parser parser(tokens);
        std::unique_ptr<Program> ast = parser.parse();
        
        // 生成IR
        std::cout << "Generating IR..." << std::endl;
        IRBuilder irBuilder(inputFile);
        std::shared_ptr<IRModule> irModule = irBuilder.build(ast.get());
        
        // 输出IR
        if (emitIR) {
            std::string irCode = irModule->toString();
            if (!writeFile(outputFile, irCode)) {
                return 1;
            }
            std::cout << "IR code written to " << outputFile << std::endl;
            return 0;
        }
        
        // 优化IR
        if (optimizationLevel > 0) {
            std::cout << "Optimizing IR (level " << optimizationLevel << ")..." << std::endl;
            Optimizer optimizer(optimizationLevel);
            irModule = optimizer.optimize(irModule);
        }
        
        // 生成目标代码
        std::cout << "Generating target code..." << std::endl;
        CodeGenerator codeGen("x86_64-unknown-linux-gnu"); // 默认目标平台
        if (!codeGen.generate(irModule, outputFile)) {
            return 1;
        }
        
        std::cout << "Compilation successful!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 