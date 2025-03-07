#include "codegen/code_generator.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace minicompiler {

CodeGenerator::CodeGenerator(const std::string& targetTriple)
    : targetTriple_(targetTriple) {}

bool CodeGenerator::generate(std::shared_ptr<IRModule> module, const std::string& outputFile) {
    std::cout << "Target triple: " << targetTriple_ << std::endl;
    
    // 寄存器分配
    for (const auto& function : module->getFunctions()) {
        allocateRegisters(function);
    }
    
    // 指令选择
    for (const auto& function : module->getFunctions()) {
        selectInstructions(function);
    }
    
    // 生成汇编代码
    std::string assembly = generateAssembly(module);
    
    // 写入输出文件
    std::ofstream outFile(outputFile);
    if (!outFile) {
        std::cerr << "Error: Could not open output file '" << outputFile << "'" << std::endl;
        return false;
    }
    
    outFile << assembly;
    outFile.close();
    
    std::cout << "Assembly code written to " << outputFile << std::endl;
    
    // TODO: 调用外部汇编器和链接器生成可执行文件
    
    return true;
}

void CodeGenerator::allocateRegisters(std::shared_ptr<IRFunction> function) {
    // TODO: 实现寄存器分配
}

void CodeGenerator::selectInstructions(std::shared_ptr<IRFunction> function) {
    // TODO: 实现指令选择
}

std::string CodeGenerator::generateAssembly(std::shared_ptr<IRModule> module) {
    // 简单地将IR转换为文本形式作为占位符
    std::stringstream ss;
    
    ss << "; Generated assembly for module: " << module->getName() << "\n";
    ss << "; Target triple: " << targetTriple_ << "\n\n";
    
    // 添加一些汇编代码的占位符
    ss << ".text\n";
    
    for (const auto& function : module->getFunctions()) {
        ss << ".global " << function->getName() << "\n";
        ss << function->getName() << ":\n";
        
        // 函数序言
        ss << "    push    %rbp\n";
        ss << "    mov     %rsp, %rbp\n";
        
        // 函数体（简单占位符）
        ss << "    ; Function body would be generated here\n";
        
        // 函数尾声
        ss << "    mov     %rbp, %rsp\n";
        ss << "    pop     %rbp\n";
        ss << "    ret\n\n";
    }
    
    return ss.str();
}

} // namespace minicompiler 