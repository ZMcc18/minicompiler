#ifndef MINICOMPILER_IR_H
#define MINICOMPILER_IR_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <ostream>

namespace minicompiler {

/**
 * @brief IR类型枚举
 */
enum class IRType {
    VOID,
    INT32,
    FLOAT32,
    POINTER,
    LABEL
};

/**
 * @brief IR操作码枚举
 */
enum class IROpcode {
    // 内存操作
    ALLOCA,     // 分配栈空间
    LOAD,       // 加载值
    STORE,      // 存储值
    
    // 算术运算
    ADD,        // 加法
    SUB,        // 减法
    MUL,        // 乘法
    DIV,        // 除法
    MOD,        // 取模
    NEG,        // 取负
    
    // 比较运算
    CMP_EQ,     // 等于
    CMP_NE,     // 不等于
    CMP_LT,     // 小于
    CMP_LE,     // 小于等于
    CMP_GT,     // 大于
    CMP_GE,     // 大于等于
    
    // 逻辑运算
    AND,        // 逻辑与
    OR,         // 逻辑或
    NOT,        // 逻辑非
    
    // 控制流
    JMP,        // 无条件跳转
    JMP_IF,     // 条件跳转
    CALL,       // 函数调用
    RET,        // 函数返回
    
    // 类型转换
    INT_TO_FLOAT,  // 整数转浮点数
    FLOAT_TO_INT,  // 浮点数转整数
    
    // 其他
    PHI,        // φ函数（用于SSA形式）
    LABEL,      // 标签
    COMMENT     // 注释（仅用于调试）
};

/**
 * @brief IR值基类
 */
class IRValue {
public:
    virtual ~IRValue() = default;
    
    /**
     * @brief 获取值的类型
     * @return IR类型
     */
    virtual IRType getType() const = 0;
    
    /**
     * @brief 获取值的字符串表示
     * @return 字符串表示
     */
    virtual std::string toString() const = 0;
};

/**
 * @brief IR常量基类
 */
class IRConstant : public IRValue {
public:
    virtual ~IRConstant() = default;
};

/**
 * @brief IR整数常量
 */
class IRIntConstant : public IRConstant {
public:
    explicit IRIntConstant(int value) : value_(value) {}
    
    int getValue() const { return value_; }
    IRType getType() const override { return IRType::INT32; }
    std::string toString() const override;
    
private:
    int value_;
};

/**
 * @brief IR浮点数常量
 */
class IRFloatConstant : public IRConstant {
public:
    explicit IRFloatConstant(float value) : value_(value) {}
    
    float getValue() const { return value_; }
    IRType getType() const override { return IRType::FLOAT32; }
    std::string toString() const override;
    
private:
    float value_;
};

/**
 * @brief IR标识符（变量、函数等）
 */
class IRIdentifier : public IRValue {
public:
    IRIdentifier(const std::string& name, IRType type)
        : name_(name), type_(type) {}
    
    const std::string& getName() const { return name_; }
    IRType getType() const override { return type_; }
    std::string toString() const override;
    
private:
    std::string name_;
    IRType type_;
};

/**
 * @brief IR标签
 */
class IRLabel : public IRValue {
public:
    explicit IRLabel(const std::string& name) : name_(name) {}
    
    const std::string& getName() const { return name_; }
    IRType getType() const override { return IRType::LABEL; }
    std::string toString() const override;
    
private:
    std::string name_;
};

/**
 * @brief IR指令
 */
class IRInstruction {
public:
    IRInstruction(IROpcode opcode, 
                 std::shared_ptr<IRIdentifier> result = nullptr,
                 std::vector<std::shared_ptr<IRValue>> operands = {})
        : opcode_(opcode), result_(result), operands_(std::move(operands)) {}
    
    IROpcode getOpcode() const { return opcode_; }
    std::shared_ptr<IRIdentifier> getResult() const { return result_; }
    const std::vector<std::shared_ptr<IRValue>>& getOperands() const { return operands_; }
    
    std::string toString() const;
    
private:
    IROpcode opcode_;
    std::shared_ptr<IRIdentifier> result_;
    std::vector<std::shared_ptr<IRValue>> operands_;
};

/**
 * @brief IR基本块
 */
class IRBasicBlock {
public:
    explicit IRBasicBlock(const std::string& name) : name_(name) {}
    
    const std::string& getName() const { return name_; }
    const std::vector<std::shared_ptr<IRInstruction>>& getInstructions() const { return instructions_; }
    
    void addInstruction(std::shared_ptr<IRInstruction> instruction) {
        instructions_.push_back(std::move(instruction));
    }
    
    std::string toString() const;
    
private:
    std::string name_;
    std::vector<std::shared_ptr<IRInstruction>> instructions_;
};

/**
 * @brief IR函数参数
 */
struct IRFunctionParameter {
    std::string name;
    IRType type;
    
    IRFunctionParameter(const std::string& n, IRType t) : name(n), type(t) {}
};

/**
 * @brief IR函数
 */
class IRFunction {
public:
    IRFunction(const std::string& name, IRType returnType, 
              std::vector<IRFunctionParameter> parameters)
        : name_(name), returnType_(returnType), parameters_(std::move(parameters)) {}
    
    const std::string& getName() const { return name_; }
    IRType getReturnType() const { return returnType_; }
    const std::vector<IRFunctionParameter>& getParameters() const { return parameters_; }
    const std::vector<std::shared_ptr<IRBasicBlock>>& getBlocks() const { return blocks_; }
    
    void addBlock(std::shared_ptr<IRBasicBlock> block) {
        blocks_.push_back(std::move(block));
    }
    
    std::string toString() const;
    
private:
    std::string name_;
    IRType returnType_;
    std::vector<IRFunctionParameter> parameters_;
    std::vector<std::shared_ptr<IRBasicBlock>> blocks_;
};

/**
 * @brief IR模块（整个程序）
 */
class IRModule {
public:
    explicit IRModule(const std::string& name) : name_(name) {}
    
    const std::string& getName() const { return name_; }
    const std::vector<std::shared_ptr<IRFunction>>& getFunctions() const { return functions_; }
    
    void addFunction(std::shared_ptr<IRFunction> function) {
        functions_.push_back(std::move(function));
    }
    
    std::string toString() const;
    
private:
    std::string name_;
    std::vector<std::shared_ptr<IRFunction>> functions_;
};

/**
 * @brief 将IRType转换为字符串
 * @param type IR类型
 * @return 字符串表示
 */
std::string irTypeToString(IRType type);

/**
 * @brief 将IROpcode转换为字符串
 * @param opcode IR操作码
 * @return 字符串表示
 */
std::string irOpcodeToString(IROpcode opcode);

} // namespace minicompiler

#endif // MINICOMPILER_IR_H 