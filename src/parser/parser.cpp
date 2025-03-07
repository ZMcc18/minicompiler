#include "parser/parser.h"
#include <iostream>
#include <sstream>

namespace minicompiler {

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

std::unique_ptr<Program> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements;
    
    try {
        while (!isAtEnd()) {
            statements.push_back(declaration());
        }
    } catch (const ParseError& e) {
        std::cerr << "Parse error: " << e.what() 
                  << " at line " << e.getLocation().line 
                  << ", column " << e.getLocation().column << std::endl;
        synchronize();
    }
    
    return std::make_unique<Program>(std::move(statements));
}

bool Parser::isAtEnd() const {
    return peek().getType() == TokenType::END_OF_FILE;
}

const Token& Parser::peek() const {
    return tokens_[current_];
}

const Token& Parser::previous() const {
    return tokens_[current_ - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) {
        current_++;
    }
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return peek().getType() == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (match(type)) {
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    
    throw error(peek(), message);
}

std::unique_ptr<Statement> Parser::declaration() {
    try {
        if (match(TokenType::INT) || match(TokenType::FLOAT)) {
            // 检查是否是函数声明
            if (check(TokenType::IDENTIFIER) && 
                tokens_[current_ + 1].getType() == TokenType::LEFT_PAREN) {
                return functionDeclaration();
            } else {
                return varDeclaration();
            }
        }
        
        if (match(TokenType::VOID)) {
            return functionDeclaration();
        }
        
        return statement();
    } catch (const ParseError& e) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Statement> Parser::varDeclaration() {
    std::string type = previous().getLexeme();
    
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    
    std::unique_ptr<Expression> initializer = nullptr;
    if (match(TokenType::ASSIGN)) {
        initializer = expression();
    }
    
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    
    return std::make_unique<VarDeclaration>(
        type, name.getLexeme(), std::move(initializer), name.getLocation());
}

std::unique_ptr<FunctionDeclaration> Parser::functionDeclaration() {
    std::string returnType = previous().getLexeme();
    
    Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
    
    consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");
    
    std::vector<FunctionParameter> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 255) {
                error(peek(), "Cannot have more than 255 parameters.");
            }
            
            std::string paramType;
            if (match(TokenType::INT)) {
                paramType = "int";
            } else if (match(TokenType::FLOAT)) {
                paramType = "float";
            } else {
                throw error(peek(), "Expect parameter type.");
            }
            
            Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            
            parameters.emplace_back(paramType, paramName.getLexeme(), paramName.getLocation());
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    
    consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");
    std::unique_ptr<BlockStatement> body = block();
    
    return std::make_unique<FunctionDeclaration>(
        returnType, name.getLexeme(), std::move(parameters), std::move(body), name.getLocation());
}

std::unique_ptr<Statement> Parser::statement() {
    if (match(TokenType::IF)) {
        return ifStatement();
    }
    
    if (match(TokenType::WHILE)) {
        return whileStatement();
    }
    
    if (match(TokenType::RETURN)) {
        return returnStatement();
    }
    
    if (match(TokenType::LEFT_BRACE)) {
        return block();
    }
    
    return expressionStatement();
}

std::unique_ptr<Statement> Parser::expressionStatement() {
    std::unique_ptr<Expression> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExpressionStatement>(std::move(expr));
}

std::unique_ptr<Statement> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    std::unique_ptr<Expression> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    
    std::unique_ptr<Statement> thenBranch = statement();
    std::unique_ptr<Statement> elseBranch = nullptr;
    
    if (match(TokenType::ELSE)) {
        elseBranch = statement();
    }
    
    return std::make_unique<IfStatement>(
        std::move(condition), std::move(thenBranch), std::move(elseBranch), previous().getLocation());
}

std::unique_ptr<Statement> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    std::unique_ptr<Expression> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
    
    std::unique_ptr<Statement> body = statement();
    
    return std::make_unique<WhileStatement>(
        std::move(condition), std::move(body), previous().getLocation());
}

std::unique_ptr<Statement> Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expression> value = nullptr;
    
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    
    return std::make_unique<ReturnStatement>(std::move(value), keyword.getLocation());
}

std::unique_ptr<BlockStatement> Parser::block() {
    std::vector<std::unique_ptr<Statement>> statements;
    SourceLocation location = previous().getLocation();
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    
    return std::make_unique<BlockStatement>(std::move(statements), location);
}

std::unique_ptr<Expression> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expression> Parser::assignment() {
    std::unique_ptr<Expression> expr = logicalOr();
    
    if (match(TokenType::ASSIGN)) {
        Token equals = previous();
        std::unique_ptr<Expression> value = assignment();
        
        if (auto* varExpr = dynamic_cast<VariableExpression*>(expr.get())) {
            return std::make_unique<BinaryExpression>(
                std::move(expr), TokenType::ASSIGN, std::move(value), equals.getLocation());
        }
        
        error(equals, "Invalid assignment target.");
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::logicalOr() {
    std::unique_ptr<Expression> expr = logicalAnd();
    
    while (match(TokenType::OR)) {
        Token op = previous();
        std::unique_ptr<Expression> right = logicalAnd();
        expr = std::make_unique<BinaryExpression>(
            std::move(expr), op.getType(), std::move(right), op.getLocation());
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::logicalAnd() {
    std::unique_ptr<Expression> expr = equality();
    
    while (match(TokenType::AND)) {
        Token op = previous();
        std::unique_ptr<Expression> right = equality();
        expr = std::make_unique<BinaryExpression>(
            std::move(expr), op.getType(), std::move(right), op.getLocation());
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::equality() {
    std::unique_ptr<Expression> expr = comparison();
    
    while (match({TokenType::EQUAL, TokenType::NOT_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expression> right = comparison();
        expr = std::make_unique<BinaryExpression>(
            std::move(expr), op.getType(), std::move(right), op.getLocation());
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::comparison() {
    std::unique_ptr<Expression> expr = term();
    
    while (match({TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER, TokenType::GREATER_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expression> right = term();
        expr = std::make_unique<BinaryExpression>(
            std::move(expr), op.getType(), std::move(right), op.getLocation());
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::term() {
    std::unique_ptr<Expression> expr = factor();
    
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = previous();
        std::unique_ptr<Expression> right = factor();
        expr = std::make_unique<BinaryExpression>(
            std::move(expr), op.getType(), std::move(right), op.getLocation());
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::factor() {
    std::unique_ptr<Expression> expr = unary();
    
    while (match({TokenType::MULTIPLY, TokenType::DIVIDE, TokenType::MODULO})) {
        Token op = previous();
        std::unique_ptr<Expression> right = unary();
        expr = std::make_unique<BinaryExpression>(
            std::move(expr), op.getType(), std::move(right), op.getLocation());
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::unary() {
    if (match({TokenType::MINUS, TokenType::NOT})) {
        Token op = previous();
        std::unique_ptr<Expression> right = unary();
        return std::make_unique<UnaryExpression>(op.getType(), std::move(right), op.getLocation());
    }
    
    return call();
}

std::unique_ptr<Expression> Parser::call() {
    std::unique_ptr<Expression> expr = primary();
    
    while (true) {
        if (match(TokenType::LEFT_PAREN)) {
            expr = finishCall(std::move(expr));
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::finishCall(std::unique_ptr<Expression> callee) {
    std::vector<std::unique_ptr<Expression>> arguments;
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                error(peek(), "Cannot have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match(TokenType::COMMA));
    }
    
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    
    // 只支持变量作为函数调用
    if (auto* varExpr = dynamic_cast<VariableExpression*>(callee.get())) {
        return std::make_unique<CallExpression>(
            varExpr->getName(), std::move(arguments), paren.getLocation());
    }
    
    throw error(paren, "Expected variable as function call target.");
}

std::unique_ptr<Expression> Parser::primary() {
    if (match(TokenType::INTEGER_LITERAL)) {
        int value = std::stoi(previous().getLexeme());
        return std::make_unique<IntegerLiteral>(value, previous().getLocation());
    }
    
    if (match(TokenType::FLOAT_LITERAL)) {
        float value = std::stof(previous().getLexeme());
        return std::make_unique<FloatLiteral>(value, previous().getLocation());
    }
    
    if (match(TokenType::STRING_LITERAL)) {
        return std::make_unique<StringLiteral>(previous().getLexeme(), previous().getLocation());
    }
    
    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<VariableExpression>(previous().getLexeme(), previous().getLocation());
    }
    
    if (match(TokenType::LEFT_PAREN)) {
        std::unique_ptr<Expression> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return expr;
    }
    
    throw error(peek(), "Expect expression.");
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().getType() == TokenType::SEMICOLON) {
            return;
        }
        
        switch (peek().getType()) {
            case TokenType::INT:
            case TokenType::FLOAT:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

ParseError Parser::error(const std::string& message) {
    return error(peek(), message);
}

ParseError Parser::error(const Token& token, const std::string& message) {
    std::ostringstream oss;
    oss << "Error at ";
    
    if (token.getType() == TokenType::END_OF_FILE) {
        oss << "end of file";
    } else {
        oss << "'" << token.getLexeme() << "'";
    }
    
    oss << ": " << message;
    
    return ParseError(oss.str(), token.getLocation());
}

} // namespace minicompiler 