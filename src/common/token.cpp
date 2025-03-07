#include "common/token.h"
#include <sstream>
#include <unordered_map>

namespace minicompiler {

// 将TokenType转换为字符串的映射表
static const std::unordered_map<TokenType, std::string> tokenTypeNames = {
    {TokenType::INT, "INT"},
    {TokenType::FLOAT, "FLOAT"},
    {TokenType::IF, "IF"},
    {TokenType::ELSE, "ELSE"},
    {TokenType::WHILE, "WHILE"},
    {TokenType::RETURN, "RETURN"},
    {TokenType::VOID, "VOID"},
    
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::INTEGER_LITERAL, "INTEGER_LITERAL"},
    {TokenType::FLOAT_LITERAL, "FLOAT_LITERAL"},
    {TokenType::STRING_LITERAL, "STRING_LITERAL"},
    
    {TokenType::PLUS, "PLUS"},
    {TokenType::MINUS, "MINUS"},
    {TokenType::MULTIPLY, "MULTIPLY"},
    {TokenType::DIVIDE, "DIVIDE"},
    {TokenType::MODULO, "MODULO"},
    {TokenType::ASSIGN, "ASSIGN"},
    {TokenType::EQUAL, "EQUAL"},
    {TokenType::NOT_EQUAL, "NOT_EQUAL"},
    {TokenType::LESS, "LESS"},
    {TokenType::LESS_EQUAL, "LESS_EQUAL"},
    {TokenType::GREATER, "GREATER"},
    {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
    {TokenType::AND, "AND"},
    {TokenType::OR, "OR"},
    {TokenType::NOT, "NOT"},
    
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::LEFT_PAREN, "LEFT_PAREN"},
    {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
    {TokenType::LEFT_BRACE, "LEFT_BRACE"},
    {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
    {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
    {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
    
    {TokenType::END_OF_FILE, "EOF"},
    {TokenType::UNKNOWN, "UNKNOWN"}
};

std::string Token::toString() const {
    std::ostringstream oss;
    
    // 获取标记类型的字符串表示
    auto it = tokenTypeNames.find(type_);
    std::string typeName = (it != tokenTypeNames.end()) ? it->second : "UNKNOWN";
    
    oss << "Token(" << typeName << ", '" << lexeme_ << "', line " 
        << location_.line << ", column " << location_.column << ")";
    
    return oss.str();
}

} // namespace minicompiler 