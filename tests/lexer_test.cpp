#include <gtest/gtest.h>
#include "lexer/lexer.h"

using namespace minicompiler;

TEST(LexerTest, EmptySource) {
    Lexer lexer("");
    auto tokens = lexer.scanTokens();
    
    ASSERT_EQ(1, tokens.size());
    EXPECT_EQ(TokenType::END_OF_FILE, tokens[0].getType());
}

TEST(LexerTest, SimpleTokens) {
    Lexer lexer("int x = 42;");
    auto tokens = lexer.scanTokens();
    
    ASSERT_EQ(6, tokens.size()); // int, x, =, 42, ;, EOF
    
    EXPECT_EQ(TokenType::INT, tokens[0].getType());
    EXPECT_EQ("int", tokens[0].getLexeme());
    
    EXPECT_EQ(TokenType::IDENTIFIER, tokens[1].getType());
    EXPECT_EQ("x", tokens[1].getLexeme());
    
    EXPECT_EQ(TokenType::ASSIGN, tokens[2].getType());
    EXPECT_EQ("=", tokens[2].getLexeme());
    
    EXPECT_EQ(TokenType::INTEGER_LITERAL, tokens[3].getType());
    EXPECT_EQ("42", tokens[3].getLexeme());
    
    EXPECT_EQ(TokenType::SEMICOLON, tokens[4].getType());
    EXPECT_EQ(";", tokens[4].getLexeme());
    
    EXPECT_EQ(TokenType::END_OF_FILE, tokens[5].getType());
}

TEST(LexerTest, Keywords) {
    Lexer lexer("int float if else while return void");
    auto tokens = lexer.scanTokens();
    
    ASSERT_EQ(8, tokens.size()); // 7 keywords + EOF
    
    EXPECT_EQ(TokenType::INT, tokens[0].getType());
    EXPECT_EQ(TokenType::FLOAT, tokens[1].getType());
    EXPECT_EQ(TokenType::IF, tokens[2].getType());
    EXPECT_EQ(TokenType::ELSE, tokens[3].getType());
    EXPECT_EQ(TokenType::WHILE, tokens[4].getType());
    EXPECT_EQ(TokenType::RETURN, tokens[5].getType());
    EXPECT_EQ(TokenType::VOID, tokens[6].getType());
}

TEST(LexerTest, Operators) {
    Lexer lexer("+ - * / % = == != < <= > >= && ||");
    auto tokens = lexer.scanTokens();
    
    ASSERT_EQ(14, tokens.size()); // 13 operators + EOF
    
    EXPECT_EQ(TokenType::PLUS, tokens[0].getType());
    EXPECT_EQ(TokenType::MINUS, tokens[1].getType());
    EXPECT_EQ(TokenType::MULTIPLY, tokens[2].getType());
    EXPECT_EQ(TokenType::DIVIDE, tokens[3].getType());
    EXPECT_EQ(TokenType::MODULO, tokens[4].getType());
    EXPECT_EQ(TokenType::ASSIGN, tokens[5].getType());
    EXPECT_EQ(TokenType::EQUAL, tokens[6].getType());
    EXPECT_EQ(TokenType::NOT_EQUAL, tokens[7].getType());
    EXPECT_EQ(TokenType::LESS, tokens[8].getType());
    EXPECT_EQ(TokenType::LESS_EQUAL, tokens[9].getType());
    EXPECT_EQ(TokenType::GREATER, tokens[10].getType());
    EXPECT_EQ(TokenType::GREATER_EQUAL, tokens[11].getType());
    EXPECT_EQ(TokenType::AND, tokens[12].getType());
}

TEST(LexerTest, Comments) {
    Lexer lexer("int x; // This is a comment\nint y; /* This is a\nmulti-line comment */ int z;");
    auto tokens = lexer.scanTokens();
    
    // int, x, ;, int, y, ;, int, z, ;, EOF
    ASSERT_EQ(10, tokens.size());
    
    EXPECT_EQ(TokenType::INT, tokens[0].getType());
    EXPECT_EQ(TokenType::IDENTIFIER, tokens[1].getType());
    EXPECT_EQ("x", tokens[1].getLexeme());
    EXPECT_EQ(TokenType::SEMICOLON, tokens[2].getType());
    
    EXPECT_EQ(TokenType::INT, tokens[3].getType());
    EXPECT_EQ(TokenType::IDENTIFIER, tokens[4].getType());
    EXPECT_EQ("y", tokens[4].getLexeme());
    EXPECT_EQ(TokenType::SEMICOLON, tokens[5].getType());
    
    EXPECT_EQ(TokenType::INT, tokens[6].getType());
    EXPECT_EQ(TokenType::IDENTIFIER, tokens[7].getType());
    EXPECT_EQ("z", tokens[7].getLexeme());
    EXPECT_EQ(TokenType::SEMICOLON, tokens[8].getType());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 