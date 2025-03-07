#include "ir/ir_builder.h"
#include <iostream>
#include <sstream>

namespace minicompiler {

IRBuilder::IRBuilder(const std::string& moduleName)
    : module_(std::make_shared<IRModule>(moduleName)) {}

std::shared_ptr<IRModule> IRBuilder::build(Program* program) {
    // 清空状态
    symbolTable_.clear();
    while (!valueStack_.empty()) {
        valueStack_.pop();
    }
    labelCounter_ = 0;
    tempCounter_ = 0;
    
    // 访问程序
    program->accept(*this);
    
    return module_;
}

void IRBuilder::visit(IntegerLiteral* node) {
    auto value = std::make_shared<IRIntConstant>(node->getValue());
    valueStack_.push(value);
}

void IRBuilder::visit(FloatLiteral* node) {
    auto value = std::make_shared<IRFloatConstant>(node->getValue());
    valueStack_.push(value);
}

void IRBuilder::visit(StringLiteral* node) {
    // 字符串字面量暂不支持
    std::cerr << "Warning: String literals are not supported in IR." << std::endl;
    
    // 使用整数0作为占位符
    auto value = std::make_shared<IRIntConstant>(0);
    valueStack_.push(value);
}

void IRBuilder::visit(VariableExpression* node) {
    const std::string& name = node->getName();
    
    // 查找变量
    auto it = symbolTable_.find(name);
    if (it == symbolTable_.end()) {
        std::cerr << "Error: Variable '" << name << "' not found." << std::endl;
        // 使用整数0作为占位符
        auto value = std::make_shared<IRIntConstant>(0);
        valueStack_.push(value);
        return;
    }
    
    // 加载变量值
    auto temp = createTemp(it->second->getType());
    auto loadInst = std::make_shared<IRInstruction>(
        IROpcode::LOAD, temp, std::vector<std::shared_ptr<IRValue>>{it->second});
    addInstruction(loadInst);
    
    valueStack_.push(temp);
}

void IRBuilder::visit(BinaryExpression* node) {
    // 访问左右操作数
    node->getLeft()->accept(*this);
    auto right = popValue();
    auto left = popValue();
    
    // 根据操作符类型创建指令
    IROpcode opcode;
    switch (node->getOperator()) {
        case TokenType::PLUS: opcode = IROpcode::ADD; break;
        case TokenType::MINUS: opcode = IROpcode::SUB; break;
        case TokenType::MULTIPLY: opcode = IROpcode::MUL; break;
        case TokenType::DIVIDE: opcode = IROpcode::DIV; break;
        case TokenType::MODULO: opcode = IROpcode::MOD; break;
        case TokenType::EQUAL: opcode = IROpcode::CMP_EQ; break;
        case TokenType::NOT_EQUAL: opcode = IROpcode::CMP_NE; break;
        case TokenType::LESS: opcode = IROpcode::CMP_LT; break;
        case TokenType::LESS_EQUAL: opcode = IROpcode::CMP_LE; break;
        case TokenType::GREATER: opcode = IROpcode::CMP_GT; break;
        case TokenType::GREATER_EQUAL: opcode = IROpcode::CMP_GE; break;
        case TokenType::AND: opcode = IROpcode::AND; break;
        case TokenType::OR: opcode = IROpcode::OR; break;
        case TokenType::ASSIGN: {
            // 特殊处理赋值操作
            if (auto* varExpr = dynamic_cast<VariableExpression*>(node->getLeft())) {
                const std::string& name = varExpr->getName();
                auto it = symbolTable_.find(name);
                if (it != symbolTable_.end()) {
                    auto storeInst = std::make_shared<IRInstruction>(
                        IROpcode::STORE, nullptr, std::vector<std::shared_ptr<IRValue>>{right, it->second});
                    addInstruction(storeInst);
                    valueStack_.push(right);
                    return;
                }
            }
            std::cerr << "Error: Invalid assignment target." << std::endl;
            valueStack_.push(right);
            return;
        }
        default:
            std::cerr << "Error: Unsupported binary operator." << std::endl;
            valueStack_.push(left);
            return;
    }
    
    // 创建临时变量存储结果
    auto temp = createTemp(left->getType());
    auto inst = std::make_shared<IRInstruction>(
        opcode, temp, std::vector<std::shared_ptr<IRValue>>{left, right});
    addInstruction(inst);
    
    valueStack_.push(temp);
}

void IRBuilder::visit(UnaryExpression* node) {
    // 访问操作数
    node->getOperand()->accept(*this);
    auto operand = popValue();
    
    // 根据操作符类型创建指令
    IROpcode opcode;
    switch (node->getOperator()) {
        case TokenType::MINUS: opcode = IROpcode::NEG; break;
        case TokenType::NOT: opcode = IROpcode::NOT; break;
        default:
            std::cerr << "Error: Unsupported unary operator." << std::endl;
            valueStack_.push(operand);
            return;
    }
    
    // 创建临时变量存储结果
    auto temp = createTemp(operand->getType());
    auto inst = std::make_shared<IRInstruction>(
        opcode, temp, std::vector<std::shared_ptr<IRValue>>{operand});
    addInstruction(inst);
    
    valueStack_.push(temp);
}

void IRBuilder::visit(CallExpression* node) {
    const std::string& callee = node->getCallee();
    
    // 处理参数
    std::vector<std::shared_ptr<IRValue>> args;
    for (const auto& arg : node->getArguments()) {
        arg->accept(*this);
        args.push_back(popValue());
    }
    
    // 创建临时变量存储结果
    auto temp = createTemp(IRType::INT32); // 假设所有函数返回整数
    auto callInst = std::make_shared<IRInstruction>(
        IROpcode::CALL, temp, args);
    addInstruction(callInst);
    
    valueStack_.push(temp);
}

void IRBuilder::visit(ExpressionStatement* node) {
    node->getExpression()->accept(*this);
    // 弹出表达式结果，因为表达式语句不需要返回值
    if (!valueStack_.empty()) {
        valueStack_.pop();
    }
}

void IRBuilder::visit(VarDeclaration* node) {
    const std::string& name = node->getName();
    IRType type = typeFromString(node->getType());
    
    // 创建变量
    auto var = std::make_shared<IRIdentifier>(name, type);
    symbolTable_[name] = var;
    
    // 分配栈空间
    auto allocaInst = std::make_shared<IRInstruction>(
        IROpcode::ALLOCA, var, std::vector<std::shared_ptr<IRValue>>{});
    addInstruction(allocaInst);
    
    // 初始化变量
    if (node->getInitializer()) {
        node->getInitializer()->accept(*this);
        auto initValue = popValue();
        
        auto storeInst = std::make_shared<IRInstruction>(
            IROpcode::STORE, nullptr, std::vector<std::shared_ptr<IRValue>>{initValue, var});
        addInstruction(storeInst);
    }
}

void IRBuilder::visit(BlockStatement* node) {
    for (const auto& stmt : node->getStatements()) {
        stmt->accept(*this);
    }
}

void IRBuilder::visit(IfStatement* node) {
    // 创建标签
    std::string thenLabel = createLabel("then");
    std::string elseLabel = createLabel("else");
    std::string endLabel = createLabel("endif");
    
    // 生成条件表达式
    node->getCondition()->accept(*this);
    auto condition = popValue();
    
    // 条件跳转
    auto jmpIfInst = std::make_shared<IRInstruction>(
        IROpcode::JMP_IF, nullptr, 
        std::vector<std::shared_ptr<IRValue>>{
            condition, 
            std::make_shared<IRLabel>(thenLabel)
        });
    addInstruction(jmpIfInst);
    
    // 无条件跳转到else分支
    auto jmpElseInst = std::make_shared<IRInstruction>(
        IROpcode::JMP, nullptr, 
        std::vector<std::shared_ptr<IRValue>>{
            std::make_shared<IRLabel>(elseLabel)
        });
    addInstruction(jmpElseInst);
    
    // then分支
    auto thenBlock = createBlock(thenLabel);
    setCurrentBlock(thenBlock);
    node->getThenBranch()->accept(*this);
    
    // 跳转到结束
    auto jmpEndInst = std::make_shared<IRInstruction>(
        IROpcode::JMP, nullptr, 
        std::vector<std::shared_ptr<IRValue>>{
            std::make_shared<IRLabel>(endLabel)
        });
    addInstruction(jmpEndInst);
    
    // else分支
    auto elseBlock = createBlock(elseLabel);
    setCurrentBlock(elseBlock);
    if (node->getElseBranch()) {
        node->getElseBranch()->accept(*this);
    }
    
    // 跳转到结束
    auto jmpEndInst2 = std::make_shared<IRInstruction>(
        IROpcode::JMP, nullptr, 
        std::vector<std::shared_ptr<IRValue>>{
            std::make_shared<IRLabel>(endLabel)
        });
    addInstruction(jmpEndInst2);
    
    // 结束标签
    auto endBlock = createBlock(endLabel);
    setCurrentBlock(endBlock);
}

void IRBuilder::visit(WhileStatement* node) {
    // 创建标签
    std::string condLabel = createLabel("while.cond");
    std::string bodyLabel = createLabel("while.body");
    std::string endLabel = createLabel("while.end");
    
    // 跳转到条件
    auto jmpCondInst = std::make_shared<IRInstruction>(
        IROpcode::JMP, nullptr, 
        std::vector<std::shared_ptr<IRValue>>{
            std::make_shared<IRLabel>(condLabel)
        });
    addInstruction(jmpCondInst);
    
    // 条件块
    auto condBlock = createBlock(condLabel);
    setCurrentBlock(condBlock);
    
    // 生成条件表达式
    node->getCondition()->accept(*this);
    auto condition = popValue();
    
    // 条件跳转
    auto jmpIfInst = std::make_shared<IRInstruction>(
        IROpcode::JMP_IF, nullptr, 
        std::vector<std::shared_ptr<IRValue>>{
            condition, 
            std::make_shared<IRLabel>(bodyLabel)
        });
    addInstruction(jmpIfInst);
    
    // 无条件跳转到结束
    auto jmpEndInst = std::make_shared<IRInstruction>(
        IROpcode::JMP, nullptr, 
        std::vector<std::shared_ptr<IRValue>>{
            std::make_shared<IRLabel>(endLabel)
        });
    addInstruction(jmpEndInst);
    
    // 循环体
    auto bodyBlock = createBlock(bodyLabel);
    setCurrentBlock(bodyBlock);
    node->getBody()->accept(*this);
    
    // 跳回条件
    auto jmpBackInst = std::make_shared<IRInstruction>(
        IROpcode::JMP, nullptr, 
        std::vector<std::shared_ptr<IRValue>>{
            std::make_shared<IRLabel>(condLabel)
        });
    addInstruction(jmpBackInst);
    
    // 结束标签
    auto endBlock = createBlock(endLabel);
    setCurrentBlock(endBlock);
}

void IRBuilder::visit(ReturnStatement* node) {
    if (node->getValue()) {
        node->getValue()->accept(*this);
        auto value = popValue();
        
        auto retInst = std::make_shared<IRInstruction>(
            IROpcode::RET, nullptr, 
            std::vector<std::shared_ptr<IRValue>>{value});
        addInstruction(retInst);
    } else {
        auto retInst = std::make_shared<IRInstruction>(
            IROpcode::RET, nullptr, 
            std::vector<std::shared_ptr<IRValue>>{});
        addInstruction(retInst);
    }
}

void IRBuilder::visit(FunctionDeclaration* node) {
    const std::string& name = node->getName();
    IRType returnType = typeFromString(node->getReturnType());
    
    // 创建函数参数
    std::vector<IRFunctionParameter> params;
    for (const auto& param : node->getParameters()) {
        IRType paramType = typeFromString(param.type);
        params.emplace_back(param.name, paramType);
    }
    
    // 创建函数
    currentFunction_ = std::make_shared<IRFunction>(name, returnType, params);
    module_->addFunction(currentFunction_);
    
    // 创建入口基本块
    auto entryBlock = createBlock("entry");
    setCurrentBlock(entryBlock);
    
    // 清空符号表
    symbolTable_.clear();
    
    // 为参数分配栈空间
    for (const auto& param : params) {
        auto var = std::make_shared<IRIdentifier>(param.name, param.type);
        symbolTable_[param.name] = var;
        
        auto allocaInst = std::make_shared<IRInstruction>(
            IROpcode::ALLOCA, var, std::vector<std::shared_ptr<IRValue>>{});
        addInstruction(allocaInst);
        
        // 将参数值存入栈
        auto paramVar = std::make_shared<IRIdentifier>("param." + param.name, param.type);
        auto storeInst = std::make_shared<IRInstruction>(
            IROpcode::STORE, nullptr, 
            std::vector<std::shared_ptr<IRValue>>{paramVar, var});
        addInstruction(storeInst);
    }
    
    // 处理函数体
    node->getBody()->accept(*this);
    
    // 如果没有显式的return语句，添加一个默认的return
    if (currentBlock_->getInstructions().empty() || 
        currentBlock_->getInstructions().back()->getOpcode() != IROpcode::RET) {
        auto retInst = std::make_shared<IRInstruction>(
            IROpcode::RET, nullptr, 
            returnType == IRType::VOID ? 
                std::vector<std::shared_ptr<IRValue>>{} : 
                std::vector<std::shared_ptr<IRValue>>{std::make_shared<IRIntConstant>(0)});
        addInstruction(retInst);
    }
    
    // 清空当前函数和基本块
    currentFunction_ = nullptr;
    currentBlock_ = nullptr;
}

void IRBuilder::visit(Program* node) {
    for (const auto& stmt : node->getStatements()) {
        stmt->accept(*this);
    }
}

std::shared_ptr<IRBasicBlock> IRBuilder::createBlock(const std::string& name) {
    auto block = std::make_shared<IRBasicBlock>(name);
    if (currentFunction_) {
        currentFunction_->addBlock(block);
    }
    return block;
}

void IRBuilder::setCurrentBlock(std::shared_ptr<IRBasicBlock> block) {
    currentBlock_ = block;
}

void IRBuilder::addInstruction(std::shared_ptr<IRInstruction> instruction) {
    if (currentBlock_) {
        currentBlock_->addInstruction(instruction);
    }
}

std::shared_ptr<IRIdentifier> IRBuilder::createTemp(IRType type, const std::string& prefix) {
    std::string name = prefix + std::to_string(tempCounter_++);
    return std::make_shared<IRIdentifier>(name, type);
}

std::string IRBuilder::createLabel(const std::string& prefix) {
    return prefix + "." + std::to_string(labelCounter_++);
}

IRType IRBuilder::typeFromString(const std::string& cType) {
    if (cType == "int") {
        return IRType::INT32;
    } else if (cType == "float") {
        return IRType::FLOAT32;
    } else if (cType == "void") {
        return IRType::VOID;
    } else {
        std::cerr << "Warning: Unknown type '" << cType << "', defaulting to INT32." << std::endl;
        return IRType::INT32;
    }
}

std::shared_ptr<IRValue> IRBuilder::popValue() {
    if (valueStack_.empty()) {
        std::cerr << "Error: Value stack is empty." << std::endl;
        return std::make_shared<IRIntConstant>(0);
    }
    
    auto value = valueStack_.top();
    valueStack_.pop();
    return value;
}

} // namespace minicompiler 