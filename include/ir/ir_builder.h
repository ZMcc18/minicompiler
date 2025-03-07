#ifndef MINICOMPILER_IR_BUILDER_H
#define MINICOMPILER_IR_BUILDER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <stack>
#include "ast/ast.h"
#include "ir/ir.h"

namespace minicompiler {

/**
 * @brief IR构建器类，负责将AST转换为IR
 */
class IRBuilder : public ASTVisitor {
public:
    /**
     * @brief 构造函数
     * @param moduleName 模块名称
     */
    explicit IRBuilder(const std::string& moduleName);
    
    /**
     * @brief 构建IR
     * @param program AST程序
     * @return IR模块
     */
    std::shared_ptr<IRModule> build(Program* program);
    
    // ASTVisitor接口实现
    void visit(IntegerLiteral* node) override;
    void visit(FloatLiteral* node) override;
    void visit(StringLiteral* node) override;
    void visit(VariableExpression* node) override;
    void visit(BinaryExpression* node) override;
    void visit(UnaryExpression* node) override;
    void visit(CallExpression* node) override;
    void visit(ExpressionStatement* node) override;
    void visit(VarDeclaration* node) override;
    void visit(BlockStatement* node) override;
    void visit(IfStatement* node) override;
    void visit(WhileStatement* node) override;
    void visit(ReturnStatement* node) override;
    void visit(FunctionDeclaration* node) override;
    void visit(Program* node) override;
    
private:
    // IR模块
    std::shared_ptr<IRModule> module_;
    
    // 当前函数
    std::shared_ptr<IRFunction> currentFunction_;
    
    // 当前基本块
    std::shared_ptr<IRBasicBlock> currentBlock_;
    
    // 符号表（变量名 -> IR标识符）
    std::unordered_map<std::string, std::shared_ptr<IRIdentifier>> symbolTable_;
    
    // 表达式结果栈
    std::stack<std::shared_ptr<IRValue>> valueStack_;
    
    // 标签计数器
    int labelCounter_ = 0;
    
    // 临时变量计数器
    int tempCounter_ = 0;
    
    // 辅助方法
    
    /**
     * @brief 创建新的基本块
     * @param name 基本块名称
     * @return 基本块
     */
    std::shared_ptr<IRBasicBlock> createBlock(const std::string& name);
    
    /**
     * @brief 设置当前基本块
     * @param block 基本块
     */
    void setCurrentBlock(std::shared_ptr<IRBasicBlock> block);
    
    /**
     * @brief 添加指令到当前基本块
     * @param instruction 指令
     */
    void addInstruction(std::shared_ptr<IRInstruction> instruction);
    
    /**
     * @brief 创建新的临时变量
     * @param type 变量类型
     * @param prefix 变量名前缀
     * @return 变量标识符
     */
    std::shared_ptr<IRIdentifier> createTemp(IRType type, const std::string& prefix = "t");
    
    /**
     * @brief 创建新的标签
     * @param prefix 标签前缀
     * @return 标签名
     */
    std::string createLabel(const std::string& prefix = "L");
    
    /**
     * @brief 将C类型转换为IR类型
     * @param cType C类型字符串
     * @return IR类型
     */
    IRType typeFromString(const std::string& cType);
    
    /**
     * @brief 获取栈顶值并弹出
     * @return 栈顶值
     */
    std::shared_ptr<IRValue> popValue();
};

} // namespace minicompiler

#endif // MINICOMPILER_IR_BUILDER_H