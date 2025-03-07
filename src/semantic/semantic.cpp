// src/semantic/semantic_analyzer.cpp
#include "semantic/semantic_analyzer.h"
#include <iostream>
#include <sstream>

namespace minicompiler {

// Scope实现
void Scope::addSymbol(const Symbol& symbol) {
    symbols_[symbol.name] = symbol;
}

Symbol* Scope::findSymbol(const std::string& name) {
    auto it = symbols_.find(name);
    if (it != symbols_.end()) {
        return &it->second;
    }
    
    if (parent_) {
        return parent_->findSymbol(name);
    }
    
    return nullptr;
}

// SemanticAnalyzer实现
SemanticAnalyzer::SemanticAnalyzer() : currentScope_(nullptr) {
    // 创建全局作用域
    enterScope();
}

void SemanticAnalyzer::analyze(Program* program) {
    // 清空状态
    while (currentScope_ && currentScope_->getParent()) {
        exitScope();
    }
    errors_.clear();
    
    // 访问程序
    program->accept(*this);
}

void SemanticAnalyzer::enterScope() {
    currentScope_ = new Scope(currentScope_);
}

void SemanticAnalyzer::exitScope() {
    if (currentScope_) {
        Scope* parent = currentScope_->getParent();
        delete currentScope_;
        currentScope_ = parent;
    }
}

void SemanticAnalyzer::addError(const std::string& message, const SourceLocation& location) {
    errors_.emplace_back(message, location);
    std::cerr << "Semantic error at line " << location.line << ", column " << location.column
              << ": " << message << std::endl;
}

std::string SemanticAnalyzer::getExpressionType(Expression* expr) {
    // 整数字面量
    if (auto* intLiteral = dynamic_cast<IntegerLiteral*>(expr)) {
        return "int";
    }
    
    // 浮点数字面量
    if (auto* floatLiteral = dynamic_cast<FloatLiteral*>(expr)) {
        return "float";
    }
    
    // 字符串字面量
    if (auto* stringLiteral = dynamic_cast<StringLiteral*>(expr)) {
        return "string";
    }
    
    // 变量引用
    if (auto* varExpr = dynamic_cast<VariableExpression*>(expr)) {
        Symbol* symbol = currentScope_->findSymbol(varExpr->getName());
        if (symbol && symbol->symbolType == SymbolType::VARIABLE) {
            return symbol->type;
        }
        return "unknown";
    }
    
    // 二元表达式
    if (auto* binaryExpr = dynamic_cast<BinaryExpression*>(expr)) {
        std::string leftType = getExpressionType(binaryExpr->getLeft());
        std::string rightType = getExpressionType(binaryExpr->getRight());
        
        TokenType op = binaryExpr->getOperator();
        
        // 比较运算符返回int（作为布尔值）
        if (op == TokenType::EQUAL || op == TokenType::NOT_EQUAL ||
            op == TokenType::LESS || op == TokenType::LESS_EQUAL ||
            op == TokenType::GREATER || op == TokenType::GREATER_EQUAL ||
            op == TokenType::AND || op == TokenType::OR) {
            return "int";
        }
        
        // 算术运算符返回操作数的共同类型
        return getCommonType(leftType, rightType);
    }
    
    // 一元表达式
    if (auto* unaryExpr = dynamic_cast<UnaryExpression*>(expr)) {
        return getExpressionType(unaryExpr->getOperand());
    }
    
    // 函数调用
    if (auto* callExpr = dynamic_cast<CallExpression*>(expr)) {
        Symbol* symbol = currentScope_->findSymbol(callExpr->getCallee());
        if (symbol && symbol->symbolType == SymbolType::FUNCTION) {
            return symbol->type;
        }
        return "unknown";
    }
    
    return "unknown";
}

bool SemanticAnalyzer::isTypeCompatible(const std::string& type1, const std::string& type2) {
    if (type1 == type2) {
        return true;
    }
    
    // int可以转换为float
    if (type1 == "int" && type2 == "float") {
        return true;
    }
    
    return false;
}

std::string SemanticAnalyzer::getCommonType(const std::string& type1, const std::string& type2) {
    if (type1 == "float" || type2 == "float") {
        return "float";
    }
    
    if (type1 == "int" && type2 == "int") {
        return "int";
    }
    
    return "unknown";
}

// ASTVisitor接口实现
void SemanticAnalyzer::visit(IntegerLiteral* node) {
    // 不需要做任何事情
}

void SemanticAnalyzer::visit(FloatLiteral* node) {
    // 不需要做任何事情
}

void SemanticAnalyzer::visit(StringLiteral* node) {
    // 不需要做任何事情
}

void SemanticAnalyzer::visit(VariableExpression* node) {
    const std::string& name = node->getName();
    Symbol* symbol = currentScope_->findSymbol(name);
    
    if (!symbol) {
        addError("Undefined variable '" + name + "'", node->getLocation());
    } else if (symbol->symbolType != SymbolType::VARIABLE) {
        addError("'" + name + "' is not a variable", node->getLocation());
    }
}

void SemanticAnalyzer::visit(BinaryExpression* node) {
    // 访问左右操作数
    node->getLeft()->accept(*this);
    node->getRight()->accept(*this);
    
    std::string leftType = getExpressionType(node->getLeft());
    std::string rightType = getExpressionType(node->getRight());
    
    // 检查类型兼容性
    TokenType op = node->getOperator();
    
    if (op == TokenType::PLUS || op == TokenType::MINUS ||
        op == TokenType::MULTIPLY || op == TokenType::DIVIDE ||
        op == TokenType::MODULO) {
        // 算术运算符
        if (leftType == "unknown" || rightType == "unknown") {
            return; // 已经报告了错误
        }
        
        if (!isTypeCompatible(leftType, rightType) && !isTypeCompatible(rightType, leftType)) {
            addError("Type mismatch in binary expression: " + leftType + " " + 
                     tokenTypeToString(op) + " " + rightType, node->getLocation());
        }
        
        // 模运算只支持整数
        if (op == TokenType::MODULO && (leftType != "int" || rightType != "int")) {
            addError("Modulo operation requires integer operands", node->getLocation());
        }
    } else if (op == TokenType::EQUAL || op == TokenType::NOT_EQUAL ||
               op == TokenType::LESS || op == TokenType::LESS_EQUAL ||
               op == TokenType::GREATER || op == TokenType::GREATER_EQUAL) {
        // 比较运算符
        if (leftType == "unknown" || rightType == "unknown") {
            return; // 已经报告了错误
        }
        
        if (!isTypeCompatible(leftType, rightType) && !isTypeCompatible(rightType, leftType)) {
            addError("Type mismatch in comparison: " + leftType + " " + 
                     tokenTypeToString(op) + " " + rightType, node->getLocation());
        }
    } else if (op == TokenType::AND || op == TokenType::OR) {
        // 逻辑运算符
        if (leftType != "int" || rightType != "int") {
            addError("Logical operators require integer (boolean) operands", node->getLocation());
        }
    } else if (op == TokenType::ASSIGN) {
        // 赋值运算符
        if (auto* varExpr = dynamic_cast<VariableExpression*>(node->getLeft())) {
            if (!isTypeCompatible(rightType, leftType)) {
                addError("Cannot assign " + rightType + " to " + leftType, node->getLocation());
            }
        } else {
            addError("Left side of assignment must be a variable", node->getLocation());
        }
    }
}

void SemanticAnalyzer::visit(UnaryExpression* node) {
    // 访问操作数
    node->getOperand()->accept(*this);
    
    std::string operandType = getExpressionType(node->getOperand());
    TokenType op = node->getOperator();
    
    if (op == TokenType::MINUS) {
        // 负号运算符
        if (operandType != "int" && operandType != "float") {
            addError("Unary minus requires numeric operand", node->getLocation());
        }
    } else if (op == TokenType::NOT) {
        // 逻辑非运算符
        if (operandType != "int") {
            addError("Logical NOT requires integer (boolean) operand", node->getLocation());
        }
    }
}

void SemanticAnalyzer::visit(CallExpression* node) {
    const std::string& callee = node->getCallee();
    Symbol* symbol = currentScope_->findSymbol(callee);
    
    if (!symbol) {
        addError("Undefined function '" + callee + "'", node->getLocation());
        return;
    }
    
    if (symbol->symbolType != SymbolType::FUNCTION) {
        addError("'" + callee + "' is not a function", node->getLocation());
        return;
    }
    
    // 检查参数数量
    const auto& args = node->getArguments();
    if (args.size() != symbol->paramTypes.size()) {
        addError("Function '" + callee + "' expects " + 
                 std::to_string(symbol->paramTypes.size()) + " arguments, but got " + 
                 std::to_string(args.size()), node->getLocation());
        return;
    }
    
    // 检查参数类型
    for (size_t i = 0; i < args.size(); ++i) {
        args[i]->accept(*this);
        std::string argType = getExpressionType(args[i].get());
        
        if (!isTypeCompatible(argType, symbol->paramTypes[i])) {
            addError("Argument " + std::to_string(i+1) + " of function '" + callee + 
                     "' expects " + symbol->paramTypes[i] + ", but got " + argType, 
                     args[i]->getLocation());
        }
    }
}

void SemanticAnalyzer::visit(ExpressionStatement* node) {
    node->getExpression()->accept(*this);
}

void SemanticAnalyzer::visit(VarDeclaration* node) {
    const std::string& name = node->getName();
    const std::string& type = node->getType();
    
    // 检查变量是否已声明
    Symbol* existingSymbol = currentScope_->findSymbol(name);
    if (existingSymbol && existingSymbol->symbolType == SymbolType::VARIABLE) {
        addError("Redefinition of variable '" + name + "'", node->getLocation());
        return;
    }
    
    // 添加变量到符号表
    Symbol symbol;
    symbol.name = name;
    symbol.type = type;
    symbol.symbolType = SymbolType::VARIABLE;
    symbol.location = node->getLocation();
    
    currentScope_->addSymbol(symbol);
    
    // 检查初始化表达式
    if (node->getInitializer()) {
        node->getInitializer()->accept(*this);
        
        std::string initType = getExpressionType(node->getInitializer());
        if (!isTypeCompatible(initType, type)) {
            addError("Cannot initialize " + type + " with " + initType, node->getLocation());
        }
    }
}

void SemanticAnalyzer::visit(BlockStatement* node) {
    // 进入新作用域
    enterScope();
    
    // 访问所有语句
    for (const auto& stmt : node->getStatements()) {
        stmt->accept(*this);
    }
    
    // 退出作用域
    exitScope();
}

void SemanticAnalyzer::visit(IfStatement* node) {
    // 检查条件表达式
    node->getCondition()->accept(*this);
    
    std::string condType = getExpressionType(node->getCondition());
    if (condType != "int") {
        addError("If condition must be an integer (boolean) expression", node->getLocation());
    }
    
    // 访问then分支
    node->getThenBranch()->accept(*this);
    
    // 访问else分支（如果有）
    if (node->getElseBranch()) {
        node->getElseBranch()->accept(*this);
    }
}

void SemanticAnalyzer::visit(WhileStatement* node) {
    // 检查条件表达式
    node->getCondition()->accept(*this);
    
    std::string condType = getExpressionType(node->getCondition());
    if (condType != "int") {
        addError("While condition must be an integer (boolean) expression", node->getLocation());
    }
    
    // 访问循环体
    node->getBody()->accept(*this);
}

void SemanticAnalyzer::visit(ReturnStatement* node) {
    // 检查是否在函数内
    if (currentFunctionReturnType_.empty()) {
        addError("Return statement outside of function", node->getLocation());
        return;
    }
    
    // 检查返回值类型
    if (node->getValue()) {
        node->getValue()->accept(*this);
        
        std::string valueType = getExpressionType(node->getValue());
        if (!isTypeCompatible(valueType, currentFunctionReturnType_)) {
            addError("Cannot return " + valueType + " from function returning " + 
                     currentFunctionReturnType_, node->getLocation());
        }
    } else if (currentFunctionReturnType_ != "void") {
        addError("Function returning " + currentFunctionReturnType_ + 
                 " must return a value", node->getLocation());
    }
}

void SemanticAnalyzer::visit(FunctionDeclaration* node) {
    const std::string& name = node->getName();
    const std::string& returnType = node->getReturnType();
    
    // 检查函数是否已声明
    Symbol* existingSymbol = currentScope_->findSymbol(name);
    if (existingSymbol) {
        addError("Redefinition of function '" + name + "'", node->getLocation());
        return;
    }
    
    // 添加函数到符号表
    Symbol symbol;
    symbol.name = name;
    symbol.type = returnType;
    symbol.symbolType = SymbolType::FUNCTION;
    symbol.location = node->getLocation();
    
    // 添加参数类型
    for (const auto& param : node->getParameters()) {
        symbol.paramTypes.push_back(param.type);
    }
    
    currentScope_->addSymbol(symbol);
    
    // 保存当前函数返回类型
    std::string prevReturnType = currentFunctionReturnType_;
    currentFunctionReturnType_ = returnType;
    
    // 进入函数作用域
    enterScope();
    
    // 添加参数到函数作用域
    for (const auto& param : node->getParameters()) {
        Symbol paramSymbol;
        paramSymbol.name = param.name;
        paramSymbol.type = param.type;
        paramSymbol.symbolType = SymbolType::VARIABLE;
        paramSymbol.location = param.location;
        
        currentScope_->addSymbol(paramSymbol);
    }
    
    // 访问函数体
    node->getBody()->accept(*this);
    
    // 退出函数作用域
    exitScope();
    
    // 恢复之前的返回类型
    currentFunctionReturnType_ = prevReturnType;
}

void SemanticAnalyzer::visit(Program* node) {
    // 添加内置函数
    Symbol printFunc;
    printFunc.name = "print";
    printFunc.type = "void";
    printFunc.symbolType = SymbolType::FUNCTION;
    printFunc.paramTypes.push_back("int");
    currentScope_->addSymbol(printFunc);
    
    // 访问所有语句
    for (const auto& stmt : node->getStatements()) {
        stmt->accept(*this);
    }
}

// 辅助函数：将TokenType转换为字符串
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::MULTIPLY: return "*";
        case TokenType::DIVIDE: return "/";
        case TokenType::MODULO: return "%";
        case TokenType::ASSIGN: return "=";
        case TokenType::EQUAL: return "==";
        case TokenType::NOT_EQUAL: return "!=";
        case TokenType::LESS: return "<";
        case TokenType::LESS_EQUAL: return "<=";
        case TokenType::GREATER: return ">";
        case TokenType::GREATER_EQUAL: return ">=";
        case TokenType::AND: return "&&";
        case TokenType::OR: return "||";
        case TokenType::NOT: return "!";
        default: return "unknown";
    }
}

} // namespace minicompiler