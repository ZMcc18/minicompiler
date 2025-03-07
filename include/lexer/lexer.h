#ifndef MINICOMPILER_LEXER_H
#define MINICOMPILER_LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "common/token.h"

namespace minicompiler {

/**
 * @brief 词法分析器类，负责将源代码转换为标记序列
 */
class Lexer {
public:
    /**
     * @brief 构造函数
     * @param source 源代码字符串
     */
    explicit Lexer(std::string source);
    
    /**
     * @brief 扫描所有标记
     * @return 标记序列
     */
    std::vector<Token> scanTokens();
    
    /**
     * @brief 扫描下一个标记
     * @return 下一个标记
     */
    Token scanToken();
    
    /**
     * @brief 获取当前位置的标记
     * @return 当前标记
     */
    const Token& peek() const;
    
    /**
     * @brief 消费当前标记并前进
     * @return 消费的标记
     */
    Token consume();
    
    /**
     * @brief 检查是否到达源代码末尾
     * @return 是否到达末尾
     */
    bool isAtEnd() const;
    
    /**
     * @brief 获取当前行号
     * @return 当前行号
     */
    int getLine() const { return line_; }
    
    /**
     * @brief 获取当前列号
     * @return 当前列号
     */
    int getColumn() const { return column_; }
    
private:
    // 源代码
    std::string source_;
    
    // 当前处理位置
    size_t start_ = 0;
    size_t current_ = 0;
    int line_ = 1;
    int column_ = 1;
    
    // 已扫描的标记
    std::vector<Token> tokens_;
    
    // 关键字映射表
    static std::unordered_map<std::string, TokenType> keywords_;
    
    // 辅助方法
    char advance();
    char peek(size_t offset = 0) const;
    bool match(char expected);
    void skipWhitespace();
    
    // 处理各种标记类型
    Token scanIdentifier();
    Token scanNumber();
    Token scanString();
    
    // 添加标记
    void addToken(TokenType type, const std::string& lexeme);
    
    // 错误处理
    void error(const std::string& message);
};

} // namespace minicompiler

#endif // MINICOMPILER_LEXER_H 