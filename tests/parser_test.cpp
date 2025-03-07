#include <gtest/gtest.h>
#include "lexer/lexer.h"
#include "parser/parser.h"

using namespace minicompiler;

TEST(ParserTest, EmptyProgram) {
    Lexer lexer("");
    auto tokens = lexer.scanTokens();
    
    Parser parser(tokens);
    auto program = parser.parse();
    
    ASSERT_NE(nullptr, program);
    EXPECT_EQ(0, program->getStatements().size());
}

TEST(ParserTest, VariableDeclaration) {
    Lexer lexer("int x = 42;");
    auto tokens = lexer.scanTokens();
    
    Parser parser(tokens);
    auto program = parser.parse();
    
    ASSERT_NE(nullptr, program);
    ASSERT_EQ(1, program->getStatements().size());
    
    auto* varDecl = dynamic_cast<VarDeclaration*>(program->getStatements()[0].get());
    ASSERT_NE(nullptr, varDecl);
    
    EXPECT_EQ("int", varDecl->getType());
    EXPECT_EQ("x", varDecl->getName());
    
    auto* initializer = dynamic_cast<IntegerLiteral*>(varDecl->getInitializer());
    ASSERT_NE(nullptr, initializer);
    EXPECT_EQ(42, initializer->getValue());
}

TEST(ParserTest, FunctionDeclaration) {
    Lexer lexer("int add(int a, int b) { return a + b; }");
    auto tokens = lexer.scanTokens();
    
    Parser parser(tokens);
    auto program = parser.parse();
    
    ASSERT_NE(nullptr, program);
    ASSERT_EQ(1, program->getStatements().size());
    
    auto* funcDecl = dynamic_cast<FunctionDeclaration*>(program->getStatements()[0].get());
    ASSERT_NE(nullptr, funcDecl);
    
    EXPECT_EQ("int", funcDecl->getReturnType());
    EXPECT_EQ("add", funcDecl->getName());
    
    const auto& params = funcDecl->getParameters();
    ASSERT_EQ(2, params.size());
    
    EXPECT_EQ("int", params[0].type);
    EXPECT_EQ("a", params[0].name);
    
    EXPECT_EQ("int", params[1].type);
    EXPECT_EQ("b", params[1].name);
    
    auto* body = funcDecl->getBody();
    ASSERT_NE(nullptr, body);
    ASSERT_EQ(1, body->getStatements().size());
    
    auto* returnStmt = dynamic_cast<ReturnStatement*>(body->getStatements()[0].get());
    ASSERT_NE(nullptr, returnStmt);
    
    auto* binaryExpr = dynamic_cast<BinaryExpression*>(returnStmt->getValue());
    ASSERT_NE(nullptr, binaryExpr);
    
    EXPECT_EQ(TokenType::PLUS, binaryExpr->getOperator());
    
    auto* left = dynamic_cast<VariableExpression*>(binaryExpr->getLeft());
    ASSERT_NE(nullptr, left);
    EXPECT_EQ("a", left->getName());
    
    auto* right = dynamic_cast<VariableExpression*>(binaryExpr->getRight());
    ASSERT_NE(nullptr, right);
    EXPECT_EQ("b", right->getName());
}

TEST(ParserTest, IfStatement) {
    Lexer lexer("if (x > 0) { y = 1; } else { y = 2; }");
    auto tokens = lexer.scanTokens();
    
    Parser parser(tokens);
    auto program = parser.parse();
    
    ASSERT_NE(nullptr, program);
    ASSERT_EQ(1, program->getStatements().size());
    
    auto* ifStmt = dynamic_cast<IfStatement*>(program->getStatements()[0].get());
    ASSERT_NE(nullptr, ifStmt);
    
    auto* condition = dynamic_cast<BinaryExpression*>(ifStmt->getCondition());
    ASSERT_NE(nullptr, condition);
    
    EXPECT_EQ(TokenType::GREATER, condition->getOperator());
    
    auto* thenBranch = dynamic_cast<BlockStatement*>(ifStmt->getThenBranch());
    ASSERT_NE(nullptr, thenBranch);
    ASSERT_EQ(1, thenBranch->getStatements().size());
    
    auto* elseBranch = dynamic_cast<BlockStatement*>(ifStmt->getElseBranch());
    ASSERT_NE(nullptr, elseBranch);
    ASSERT_EQ(1, elseBranch->getStatements().size());
}

TEST(ParserTest, WhileStatement) {
    Lexer lexer("while (i < 10) { i = i + 1; }");
    auto tokens = lexer.scanTokens();
    
    Parser parser(tokens);
    auto program = parser.parse();
    
    ASSERT_NE(nullptr, program);
    ASSERT_EQ(1, program->getStatements().size());
    
    auto* whileStmt = dynamic_cast<WhileStatement*>(program->getStatements()[0].get());
    ASSERT_NE(nullptr, whileStmt);
    
    auto* condition = dynamic_cast<BinaryExpression*>(whileStmt->getCondition());
    ASSERT_NE(nullptr, condition);
    
    EXPECT_EQ(TokenType::LESS, condition->getOperator());
    
    auto* body = dynamic_cast<BlockStatement*>(whileStmt->getBody());
    ASSERT_NE(nullptr, body);
    ASSERT_EQ(1, body->getStatements().size());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 