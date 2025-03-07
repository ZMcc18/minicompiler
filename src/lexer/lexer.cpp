#include "lexer/lexer.h"
#include <iostream>
#include <cctype>
#include <stdexcept>

namespace minicompiler {

// 初始化关键字映射表
std::unordered_map<std::string, TokenType> Lexer::keywords_ = {
    {"int", TokenType::INT},
    {"float", TokenType::FLOAT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"return", TokenType::RETURN},
    {"void", TokenType::VOID}
};

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start_ = current_;
        tokens_.push_back(scanToken());
    }
    
    // 添加文件结束标记
    tokens_.emplace_back(TokenType::END_OF_FILE, "", SourceLocation(line_, column_));
    return tokens_;
}

Token Lexer::scanToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::END_OF_FILE, "", SourceLocation(line_, column_));
    }
    
    start_ = current_;
    char c = advance();
    
    // 标识符
    if (std::isalpha(c) || c == '_') {
        return scanIdentifier();
    }
    
    // 数字
    if (std::isdigit(c)) {
        return scanNumber();
    }
    
    // 字符串
    if (c == '"') {
        return scanString();
    }
    
    // 运算符和分隔符
    switch (c) {
        case '(': return Token(TokenType::LEFT_PAREN, "(", SourceLocation(line_, column_ - 1));
        case ')': return Token(TokenType::RIGHT_PAREN, ")", SourceLocation(line_, column_ - 1));
        case '{': return Token(TokenType::LEFT_BRACE, "{", SourceLocation(line_, column_ - 1));
        case '}': return Token(TokenType::RIGHT_BRACE, "}", SourceLocation(line_, column_ - 1));
        case '[': return Token(TokenType::LEFT_BRACKET, "[", SourceLocation(line_, column_ - 1));
        case ']': return Token(TokenType::RIGHT_BRACKET, "]", SourceLocation(line_, column_ - 1));
        case ',': return Token(TokenType::COMMA, ",", SourceLocation(line_, column_ - 1));
        case '.': return Token(TokenType::UNKNOWN, ".", SourceLocation(line_, column_ - 1));
        case ';': return Token(TokenType::SEMICOLON, ";", SourceLocation(line_, column_ - 1));
        case '+': return Token(TokenType::PLUS, "+", SourceLocation(line_, column_ - 1));
        case '-': return Token(TokenType::MINUS, "-", SourceLocation(line_, column_ - 1));
        case '*': return Token(TokenType::MULTIPLY, "*", SourceLocation(line_, column_ - 1));
        case '/': return Token(TokenType::DIVIDE, "/", SourceLocation(line_, column_ - 1));
        case '%': return Token(TokenType::MODULO, "%", SourceLocation(line_, column_ - 1));
        
        // 双字符运算符
        case '=':
            if (match('=')) {
                return Token(TokenType::EQUAL, "==", SourceLocation(line_, column_ - 2));
            } else {
                return Token(TokenType::ASSIGN, "=", SourceLocation(line_, column_ - 1));
            }
        case '!':
            if (match('=')) {
                return Token(TokenType::NOT_EQUAL, "!=", SourceLocation(line_, column_ - 2));
            } else {
                return Token(TokenType::NOT, "!", SourceLocation(line_, column_ - 1));
            }
        case '<':
            if (match('=')) {
                return Token(TokenType::LESS_EQUAL, "<=", SourceLocation(line_, column_ - 2));
            } else {
                return Token(TokenType::LESS, "<", SourceLocation(line_, column_ - 1));
            }
        case '>':
            if (match('=')) {
                return Token(TokenType::GREATER_EQUAL, ">=", SourceLocation(line_, column_ - 2));
            } else {
                return Token(TokenType::GREATER, ">", SourceLocation(line_, column_ - 1));
            }
        case '&':
            if (match('&')) {
                return Token(TokenType::AND, "&&", SourceLocation(line_, column_ - 2));
            } else {
                return Token(TokenType::UNKNOWN, "&", SourceLocation(line_, column_ - 1));
            }
        case '|':
            if (match('|')) {
                return Token(TokenType::OR, "||", SourceLocation(line_, column_ - 2));
            } else {
                return Token(TokenType::UNKNOWN, "|", SourceLocation(line_, column_ - 1));
            }
    }
    
    // 未知字符
    return Token(TokenType::UNKNOWN, std::string(1, c), SourceLocation(line_, column_ - 1));
}

const Token& Lexer::peek() const {
    if (tokens_.empty()) {
        throw std::runtime_error("No tokens available");
    }
    return tokens_.front();
}

Token Lexer::consume() {
    if (tokens_.empty()) {
        throw std::runtime_error("No tokens available");
    }
    Token token = tokens_.front();
    tokens_.erase(tokens_.begin());
    return token;
}

bool Lexer::isAtEnd() const {
    return current_ >= source_.length();
}

char Lexer::advance() {
    column_++;
    return source_[current_++];
}

char Lexer::peek(size_t offset) const {
    if (current_ + offset >= source_.length()) {
        return '\0';
    }
    return source_[current_ + offset];
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source_[current_] != expected) {
        return false;
    }
    
    current_++;
    column_++;
    return true;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
                advance();
                break;
            case '\n':
                line_++;
                column_ = 1;
                advance();
                break;
            case '/':
                if (peek(1) == '/') {
                    // 单行注释
                    while (peek() != '\n' && !isAtEnd()) {
                        advance();
                    }
                } else if (peek(1) == '*') {
                    // 多行注释
                    advance(); // 消费 '/'
                    advance(); // 消费 '*'
                    
                    while (!isAtEnd() && !(peek() == '*' && peek(1) == '/')) {
                        if (peek() == '\n') {
                            line_++;
                            column_ = 1;
                        }
                        advance();
                    }
                    
                    if (!isAtEnd()) {
                        advance(); // 消费 '*'
                        advance(); // 消费 '/'
                    }
                } else {
                    return; // 不是注释，是除法运算符
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::scanIdentifier() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }
    
    std::string text = source_.substr(start_, current_ - start_);
    TokenType type = TokenType::IDENTIFIER;
    
    // 检查是否是关键字
    auto it = keywords_.find(text);
    if (it != keywords_.end()) {
        type = it->second;
    }
    
    return Token(type, text, SourceLocation(line_, column_ - text.length()));
}

Token Lexer::scanNumber() {
    bool isFloat = false;
    
    while (std::isdigit(peek())) {
        advance();
    }
    
    // 小数部分
    if (peek() == '.' && std::isdigit(peek(1))) {
        isFloat = true;
        advance(); // 消费 '.'
        
        while (std::isdigit(peek())) {
            advance();
        }
    }
    
    std::string text = source_.substr(start_, current_ - start_);
    TokenType type = isFloat ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL;
    
    return Token(type, text, SourceLocation(line_, column_ - text.length()));
}

Token Lexer::scanString() {
    // 跳过开始的引号
    
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line_++;
            column_ = 1;
        }
        advance();
    }
    
    if (isAtEnd()) {
        // 字符串未闭合
        error("Unterminated string.");
        return Token(TokenType::UNKNOWN, source_.substr(start_, current_ - start_), 
                    SourceLocation(line_, column_ - (current_ - start_)));
    }
    
    // 消费结束的引号
    advance();
    
    // 提取字符串内容（不包括引号）
    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    return Token(TokenType::STRING_LITERAL, value, SourceLocation(line_, column_ - value.length() - 2));
}

void Lexer::addToken(TokenType type, const std::string& lexeme) {
    tokens_.emplace_back(type, lexeme, SourceLocation(line_, column_ - lexeme.length()));
}

void Lexer::error(const std::string& message) {
    std::cerr << "Error at line " << line_ << ", column " << column_ << ": " << message << std::endl;
}

} // namespace minicompiler 