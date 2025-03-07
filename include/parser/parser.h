#ifndef MINICOMPILER_PARSER_H
#define MINICOMPILER_PARSER_H

#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include "lexer/lexer.h"
#include "ast/ast.h"

namespace minicompiler {

/**
 * @brief 解析错误异常类
 */
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message, const SourceLocation& location)
        : std::runtime_error(message), location_(location) {}
    
    SourceLocation getLocation() const { return location_; }
    
private:
    SourceLocation location_;
};

/**
 * @brief 语法分析器类，负责将标记序列转换为AST
 */
class Parser {
public:
    /**
     * @brief 构造函数
     * @param tokens 标记序列
     */
    explicit Parser(std::vector<Token> tokens);
    
    /**
     * @brief 解析程序
     * @return 程序AST
     */
    std::unique_ptr<Program> parse();
    
private:
    // 标记序列
    std::vector<Token> tokens_;
    
    // 当前处理位置
    size_t current_ = 0;
    
    // 辅助方法
    bool isAtEnd() const;
    const Token& peek() const;
    const Token& previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    Token consume(TokenType type, const std::string& message);
    
    // 递归下降解析方法
    std::unique_ptr<Statement> declaration();
    std::unique_ptr<Statement> varDeclaration();
    std::unique_ptr<FunctionDeclaration> functionDeclaration();
    std::unique_ptr<Statement> statement();
    std::unique_ptr<Statement> expressionStatement();
    std::unique_ptr<Statement> ifStatement();
    std::unique_ptr<Statement> whileStatement();
    std::unique_ptr<Statement> returnStatement();
    std::unique_ptr<BlockStatement> block();
    
    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> assignment();
    std::unique_ptr<Expression> logicalOr();
    std::unique_ptr<Expression> logicalAnd();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> call();
    std::unique_ptr<Expression> primary();
    
    std::unique_ptr<Expression> finishCall(std::unique_ptr<Expression> callee);
    
    // 错误处理
    void synchronize();
    ParseError error(const std::string& message);
    ParseError error(const Token& token, const std::string& message);
};

} // namespace minicompiler

#endif // MINICOMPILER_PARSER_H 