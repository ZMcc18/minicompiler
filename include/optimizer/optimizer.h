#ifndef MINICOMPILER_OPTIMIZER_H
#define MINICOMPILER_OPTIMIZER_H

#include <memory>
#include "ir/ir.h"

namespace minicompiler {

/**
 * @brief 优化器类，负责对IR进行优化
 */
class Optimizer {
public:
    /**
     * @brief 构造函数
     * @param level 优化级别（0-2）
     */
    explicit Optimizer(int level);
    
    /**
     * @brief 优化IR模块
     * @param module 待优化的IR模块
     * @return 优化后的IR模块
     */
    std::shared_ptr<IRModule> optimize(std::shared_ptr<IRModule> module);
    
private:
    int level_;
    
    // 各种优化pass
    void constantFolding(std::shared_ptr<IRModule> module);
    void deadCodeElimination(std::shared_ptr<IRModule> module);
    void commonSubexpressionElimination(std::shared_ptr<IRModule> module);
    void loopInvariantCodeMotion(std::shared_ptr<IRModule> module);
    void functionInlining(std::shared_ptr<IRModule> module);
};

} // namespace minicompiler

#endif // MINICOMPILER_OPTIMIZER_H 