#ifndef MINICOMPILER_TOKEN_H
#define MINICOMPILER_TOKEN_H

#include <string>
#include <utility>

namespace minicompiler {

/**
 * @brief 标记类型枚举
 */
enum class TokenType {
    // 关键字
    INT,
    FLOAT,
    IF,
    ELSE,
    WHILE,
    RETURN,
    VOID,
    
    // 标识符和字面量
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    
    // 运算符
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    ASSIGN,         // =
    EQUAL,          // ==
    NOT_EQUAL,      // !=
    LESS,           // <
    LESS_EQUAL,     // <=
    GREATER,        // >
    GREATER_EQUAL,  // >=
    AND,            // &&
    OR,             // ||
    NOT,            // !
    
    // 分隔符
    SEMICOLON,      // ;
    COMMA,          // ,
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    
    // 特殊标记
    END_OF_FILE,
    UNKNOWN
};

/**
 * @brief 表示源代码中的位置
 */
struct SourceLocation {
    int line;
    int column;
    
    SourceLocation(int l = 0, int c = 0) : line(l), column(c) {}
};

/**
 * @brief 标记类，表示词法分析的基本单位
 */
class Token {
public:
    Token(TokenType type, std::string lexeme, SourceLocation location)
        : type_(type), lexeme_(std::move(lexeme)), location_(location) {}
    
    TokenType getType() const { return type_; }
    const std::string& getLexeme() const { return lexeme_; }
    const SourceLocation& getLocation() const { return location_; }
    
    bool isKeyword() const {
        return type_ == TokenType::INT || 
               type_ == TokenType::FLOAT || 
               type_ == TokenType::IF || 
               type_ == TokenType::ELSE || 
               type_ == TokenType::WHILE || 
               type_ == TokenType::RETURN ||
               type_ == TokenType::VOID;
    }
    
    bool isOperator() const {
        return type_ == TokenType::PLUS || 
               type_ == TokenType::MINUS || 
               type_ == TokenType::MULTIPLY || 
               type_ == TokenType::DIVIDE || 
               type_ == TokenType::MODULO || 
               type_ == TokenType::ASSIGN || 
               type_ == TokenType::EQUAL || 
               type_ == TokenType::NOT_EQUAL || 
               type_ == TokenType::LESS || 
               type_ == TokenType::LESS_EQUAL || 
               type_ == TokenType::GREATER || 
               type_ == TokenType::GREATER_EQUAL || 
               type_ == TokenType::AND || 
               type_ == TokenType::OR || 
               type_ == TokenType::NOT;
    }
    
    std::string toString() const;
    
private:
    TokenType type_;
    std::string lexeme_;
    SourceLocation location_;
};

} // namespace minicompiler

#endif // MINICOMPILER_TOKEN_H 