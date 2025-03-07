#ifndef MINICOMPILER_AST_H
#define MINICOMPILER_AST_H

#include <string>
#include <vector>
#include <memory>
#include "common/token.h"

namespace minicompiler {

// 前向声明
class ASTVisitor;

/**
 * @brief AST节点基类
 */
class ASTNode {
public:
    virtual ~ASTNode() = default;
    
    /**
     * @brief 接受访问者模式的访问方法
     * @param visitor 访问者对象
     */
    virtual void accept(ASTVisitor& visitor) = 0;
    
    /**
     * @brief 获取节点的源代码位置
     * @return 源代码位置
     */
    virtual SourceLocation getLocation() const = 0;
};

/**
 * @brief 表达式节点基类
 */
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

/**
 * @brief 语句节点基类
 */
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

/**
 * @brief 字面量表达式基类
 */
class Literal : public Expression {
public:
    virtual ~Literal() = default;
};

/**
 * @brief 整数字面量
 */
class IntegerLiteral : public Literal {
public:
    IntegerLiteral(int value, const SourceLocation& location)
        : value_(value), location_(location) {}
    
    int getValue() const { return value_; }
    SourceLocation getLocation() const override { return location_; }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    int value_;
    SourceLocation location_;
};

/**
 * @brief 浮点数字面量
 */
class FloatLiteral : public Literal {
public:
    FloatLiteral(float value, const SourceLocation& location)
        : value_(value), location_(location) {}
    
    float getValue() const { return value_; }
    SourceLocation getLocation() const override { return location_; }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    float value_;
    SourceLocation location_;
};

/**
 * @brief 字符串字面量
 */
class StringLiteral : public Literal {
public:
    StringLiteral(const std::string& value, const SourceLocation& location)
        : value_(value), location_(location) {}
    
    const std::string& getValue() const { return value_; }
    SourceLocation getLocation() const override { return location_; }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    std::string value_;
    SourceLocation location_;
};

/**
 * @brief 变量引用表达式
 */
class VariableExpression : public Expression {
public:
    VariableExpression(const std::string& name, const SourceLocation& location)
        : name_(name), location_(location) {}
    
    const std::string& getName() const { return name_; }
    SourceLocation getLocation() const override { return location_; }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    std::string name_;
    SourceLocation location_;
};

/**
 * @brief 二元操作表达式
 */
class BinaryExpression : public Expression {
public:
    BinaryExpression(std::unique_ptr<Expression> left, TokenType op, 
                    std::unique_ptr<Expression> right, const SourceLocation& location)
        : left_(std::move(left)), operator_(op), right_(std::move(right)), location_(location) {}
    
    Expression* getLeft() const { return left_.get(); }
    TokenType getOperator() const { return operator_; }
    Expression* getRight() const { return right_.get(); }
    SourceLocation getLocation() const override { return location_; }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    std::unique_ptr<Expression> left_;
    TokenType operator_;
    std::unique_ptr<Expression> right_;
    SourceLocation location_;
};

/**
 * @brief 一元操作表达式
 */
class UnaryExpression : public Expression {
public:
    UnaryExpression(TokenType op, std::unique_ptr<Expression> operand, const SourceLocation& location)
        : operator_(op), operand_(std::move(operand)), location_(location) {}
    
    TokenType getOperator() const { return operator_; }
    Expression* getOperand() const { return operand_.get(); }
    SourceLocation getLocation() const override { return location_; }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    TokenType operator_;
    std::unique_ptr<Expression> operand_;
    SourceLocation location_;
};

/**
 * @brief 函数调用表达式
 */
class CallExpression : public Expression {
public:
    CallExpression(const std::string& callee, std::vector<std::unique_ptr<Expression>> arguments,
                  const SourceLocation& location)
        : callee_(callee), arguments_(std::move(arguments)), location_(location) {}
    
    const std::string& getCallee() const { return callee_; }
    const std::vector<std::unique_ptr<Expression>>& getArguments() const { return arguments_; }
    SourceLocation getLocation() const override { return location_; }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    std::string callee_;
    std::vector<std::unique_ptr<Expression>> arguments_;
    SourceLocation location_;
};

/**
 * @brief 表达式语句
 */
class ExpressionStatement : public Statement {
public:
    ExpressionStatement(std::unique_ptr<Expression> expression)
        : expression_(std::move(expression)) {}
    
    Expression* getExpression() const { return expression_.get(); }
    SourceLocation getLocation() const override { return expression_->getLocation(); }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    std::unique_ptr<Expression> expression_;
};

/**
 * @brief 变量声明语句
 */
class VarDeclaration : public Statement {
public:
    VarDeclaration(const std::string& type, const std::string& name, 
                  std::unique_ptr<Expression> initializer, const SourceLocation& location)
        : type_(type), name_(name), initializer_(std::move(initializer)), location_(location) {}
    
    const std::string& getType() const { return type_; }
    const std::string& getName() const { return name_; }
    Expression* getInitializer() const { return initializer_.get(); }
    SourceLocation getLocation() const override { return location_; }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    std::string type_;
    std::string name_;
    std::unique_ptr<Expression> initializer_;
    SourceLocation location_;
};

/**
 * @brief 块语句
 */
class BlockStatement : public Statement {
public:
    BlockStatement(std::vector<std::unique_ptr<Statement>> statements, const SourceLocation& location)
        : statements_(std::move(statements)), location_(location) {}
    
    const std::vector<std::unique_ptr<Statement>>& getStatements() const { return statements_; }
    SourceLocation getLocation() const override { return location_; }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    std::vector<std::unique_ptr<Statement>> statements_;
    SourceLocation location_;
};

/**
 * @brief If语句
 */
class IfStatement : public Statement {
public:
    IfStatement(std::unique_ptr<Expression> condition, 
               std::unique_ptr<Statement> thenBranch,
               std::unique_ptr<Statement> elseBranch,
               const SourceLocation& location)
        : condition_(std::move(condition)), 
          thenBranch_(std::move(thenBranch)), 
          elseBranch_(std::move(elseBranch)), 
          location_(location) {}
    
    Expression* getCondition() const { return condition_.get(); }
    Statement* getThenBranch() const { return thenBranch_.get(); }
    Statement* getElseBranch() const { return elseBranch_.get(); }
    SourceLocation getLocation() const override { return location_; }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Statement> thenBranch_;
    std::unique_ptr<Statement> elseBranch_;
    SourceLocation location_;
};

/**
 * @brief While语句
 */
class WhileStatement : public Statement {
public:
    WhileStatement(std::unique_ptr<Expression> condition, 
                  std::unique_ptr<Statement> body,
                  const SourceLocation& location)
        : condition_(std::move(condition)), body_(std::move(body)), location_(location) {}
    
    Expression* getCondition() const { return condition_.get(); }
    Statement* getBody() const { return body_.get(); }
    SourceLocation getLocation() const override { return location_; }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Statement> body_;
    SourceLocation location_;
};

/**
 * @brief Return语句
 */
class ReturnStatement : public Statement {
public:
    ReturnStatement(std::unique_ptr<Expression> value, const SourceLocation& location)
        : value_(std::move(value)), location_(location) {}
    
    Expression* getValue() const { return value_.get(); }
    SourceLocation getLocation() const override { return location_; }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    std::unique_ptr<Expression> value_;
    SourceLocation location_;
};

/**
 * @brief 函数参数
 */
struct FunctionParameter {
    std::string type;
    std::string name;
    SourceLocation location;
    
    FunctionParameter(const std::string& t, const std::string& n, const SourceLocation& loc)
        : type(t), name(n), location(loc) {}
};

/**
 * @brief 函数声明
 */
class FunctionDeclaration : public Statement {
public:
    FunctionDeclaration(const std::string& returnType, 
                       const std::string& name,
                       std::vector<FunctionParameter> parameters,
                       std::unique_ptr<BlockStatement> body,
                       const SourceLocation& location)
        : returnType_(returnType), 
          name_(name), 
          parameters_(std::move(parameters)), 
          body_(std::move(body)), 
          location_(location) {}
    
    const std::string& getReturnType() const { return returnType_; }
    const std::string& getName() const { return name_; }
    const std::vector<FunctionParameter>& getParameters() const { return parameters_; }
    BlockStatement* getBody() const { return body_.get(); }
    SourceLocation getLocation() const override { return location_; }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    std::string returnType_;
    std::string name_;
    std::vector<FunctionParameter> parameters_;
    std::unique_ptr<BlockStatement> body_;
    SourceLocation location_;
};

/**
 * @brief 程序
 */
class Program : public ASTNode {
public:
    Program(std::vector<std::unique_ptr<Statement>> statements)
        : statements_(std::move(statements)) {}
    
    const std::vector<std::unique_ptr<Statement>>& getStatements() const { return statements_; }
    SourceLocation getLocation() const override { 
        return statements_.empty() ? SourceLocation() : statements_[0]->getLocation(); 
    }
    
    void accept(ASTVisitor& visitor) override;
    
private:
    std::vector<std::unique_ptr<Statement>> statements_;
};

/**
 * @brief AST访问者接口
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit(IntegerLiteral* node) = 0;
    virtual void visit(FloatLiteral* node) = 0;
    virtual void visit(StringLiteral* node) = 0;
    virtual void visit(VariableExpression* node) = 0;
    virtual void visit(BinaryExpression* node) = 0;
    virtual void visit(UnaryExpression* node) = 0;
    virtual void visit(CallExpression* node) = 0;
    virtual void visit(ExpressionStatement* node) = 0;
    virtual void visit(VarDeclaration* node) = 0;
    virtual void visit(BlockStatement* node) = 0;
    virtual void visit(IfStatement* node) = 0;
    virtual void visit(WhileStatement* node) = 0;
    virtual void visit(ReturnStatement* node) = 0;
    virtual void visit(FunctionDeclaration* node) = 0;
    virtual void visit(Program* node) = 0;
};

} // namespace minicompiler

#endif // MINICOMPILER_AST_H 