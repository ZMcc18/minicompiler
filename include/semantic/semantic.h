// include/semantic/semantic_analyzer.h
#ifndef MINICOMPILER_SEMANTIC_ANALYZER_H
#define MINICOMPILER_SEMANTIC_ANALYZER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "ast/ast.h"

namespace minicompiler {

// 符号类型
enum class SymbolType {
    VARIABLE,
    FUNCTION
};

// 符号信息
struct Symbol {
    std::string name;
    std::string type;
    SymbolType symbolType;
    SourceLocation location;
    
    // 函数特有信息
    std::vector<std::string> paramTypes;
};

// 作用域
class Scope {
public:
    Scope(Scope* parent = nullptr) : parent_(parent) {}
    
    // 添加符号
    void addSymbol(const Symbol& symbol);
    
    // 查找符号
    Symbol* findSymbol(const std::string& name);
    
    // 获取父作用域
    Scope* getParent() const { return parent_; }
    
private:
    Scope* parent_;
    std::unordered_map<std::string, Symbol> symbols_;
};

// 语义错误
class SemanticError : public std::runtime_error {
public:
    SemanticError(const std::string& message, const SourceLocation& location)
        : std::runtime_error(message), location_(location) {}
    
    SourceLocation getLocation() const { return location_; }
    
private:
    SourceLocation location_;
};

// 语义分析器
class SemanticAnalyzer : public ASTVisitor {
public:
    SemanticAnalyzer();
    
    // 分析AST
    void analyze(Program* program);
    
    // 获取错误信息
    const std::vector<SemanticError>& getErrors() const { return errors_; }
    
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
    // 当前作用域
    Scope* currentScope_;
    
    // 当前函数返回类型
    std::string currentFunctionReturnType_;
    
    // 错误列表
    std::vector<SemanticError> errors_;
    
    // 辅助方法
    void enterScope();
    void exitScope();
    void addError(const std::string& message, const SourceLocation& location);
    
    // 类型检查
    std::string getExpressionType(Expression* expr);
    bool isTypeCompatible(const std::string& type1, const std::string& type2);
    std::string getCommonType(const std::string& type1, const std::string& type2);
};

} // namespace minicompiler

#endif // MINICOMPILER_SEMANTIC_ANALYZER_H