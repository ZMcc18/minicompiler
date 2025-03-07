#ifndef MINICOMPILER_CODE_GENERATOR_H
#define MINICOMPILER_CODE_GENERATOR_H

#include <string>
#include <memory>
#include "ir/ir.h"

namespace minicompiler {

/**
 * @brief 代码生成器类，负责将IR转换为目标代码
 */
class CodeGenerator {
public:
    /**
     * @brief 构造函数
     * @param targetTriple 目标平台三元组
     */
    explicit CodeGenerator(const std::string& targetTriple);
    
    /**
     * @brief 生成目标代码
     * @param module IR模块
     * @param outputFile 输出文件路径
     * @return 是否成功
     */
    bool generate(std::shared_ptr<IRModule> module, const std::string& outputFile);
    
private:
    std::string targetTriple_;
    
    // 寄存器分配
    void allocateRegisters(std::shared_ptr<IRFunction> function);
    
    // 指令选择
    void selectInstructions(std::shared_ptr<IRFunction> function);
    
    // 生成汇编代码
    std::string generateAssembly(std::shared_ptr<IRModule> module);
};

} // namespace minicompiler

#endif // MINICOMPILER_CODE_GENERATOR_H 