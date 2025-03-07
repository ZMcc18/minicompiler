#include <gtest/gtest.h>
#include <sstream>
#include "ir/ir.h"
#include "ir/ir_builder.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

using namespace minicompiler;

TEST(IRTest, IRConstantToString) {
    auto intConst = std::make_shared<IRIntConstant>(42);
    EXPECT_EQ("42", intConst->toString());
    
    auto floatConst = std::make_shared<IRFloatConstant>(3.14159f);
    EXPECT_EQ("3.141590", floatConst->toString());
}

TEST(IRTest, IRIdentifierToString) {
    auto ident = std::make_shared<IRIdentifier>("foo", IRType::INT32);
    EXPECT_EQ("%foo", ident->toString());
}

TEST(IRTest, IRLabelToString) {
    auto label = std::make_shared<IRLabel>("loop");
    EXPECT_EQ("loop:", label->toString());
}

TEST(IRTest, IRInstructionToString) {
    auto result = std::make_shared<IRIdentifier>("result", IRType::INT32);
    auto op1 = std::make_shared<IRIdentifier>("a", IRType::INT32);
    auto op2 = std::make_shared<IRIdentifier>("b", IRType::INT32);
    
    auto inst = std::make_shared<IRInstruction>(
        IROpcode::ADD, result, std::vector<std::shared_ptr<IRValue>>{op1, op2});
    
    EXPECT_EQ("%result = add %a, %b", inst->toString());
}

TEST(IRTest, IRBasicBlockToString) {
    auto block = std::make_shared<IRBasicBlock>("entry");
    
    auto result = std::make_shared<IRIdentifier>("result", IRType::INT32);
    auto op1 = std::make_shared<IRIdentifier>("a", IRType::INT32);
    auto op2 = std::make_shared<IRIdentifier>("b", IRType::INT32);
    
    auto inst = std::make_shared<IRInstruction>(
        IROpcode::ADD, result, std::vector<std::shared_ptr<IRValue>>{op1, op2});
    
    block->addInstruction(inst);
    
    std::string expected = "entry:\n  %result = add %a, %b\n";
    EXPECT_EQ(expected, block->toString());
}

TEST(IRTest, IRFunctionToString) {
    std::vector<IRFunctionParameter> params = {
        {"a", IRType::INT32},
        {"b", IRType::INT32}
    };
    
    auto func = std::make_shared<IRFunction>("add", IRType::INT32, params);
    
    auto entryBlock = std::make_shared<IRBasicBlock>("entry");
    
    auto result = std::make_shared<IRIdentifier>("result", IRType::INT32);
    auto op1 = std::make_shared<IRIdentifier>("a", IRType::INT32);
    auto op2 = std::make_shared<IRIdentifier>("b", IRType::INT32);
    
    auto addInst = std::make_shared<IRInstruction>(
        IROpcode::ADD, result, std::vector<std::shared_ptr<IRValue>>{op1, op2});
    
    entryBlock->addInstruction(addInst);
    
    auto retInst = std::make_shared<IRInstruction>(
        IROpcode::RET, nullptr, std::vector<std::shared_ptr<IRValue>>{result});
    
    entryBlock->addInstruction(retInst);
    
    func->addBlock(entryBlock);
    
    std::string expected = "define i32 @add(i32 %a, i32 %b) {\n"
                          "entry:\n"
                          "  %result = add %a, %b\n"
                          "  ret %result\n"
                          "}\n";
    
    EXPECT_EQ(expected, func->toString());
}

TEST(IRTest, IRModuleToString) {
    auto module = std::make_shared<IRModule>("test_module");
    
    std::vector<IRFunctionParameter> params = {
        {"a", IRType::INT32},
        {"b", IRType::INT32}
    };
    
    auto func = std::make_shared<IRFunction>("add", IRType::INT32, params);
    
    auto entryBlock = std::make_shared<IRBasicBlock>("entry");
    
    auto result = std::make_shared<IRIdentifier>("result", IRType::INT32);
    auto op1 = std::make_shared<IRIdentifier>("a", IRType::INT32);
    auto op2 = std::make_shared<IRIdentifier>("b", IRType::INT32);
    
    auto addInst = std::make_shared<IRInstruction>(
        IROpcode::ADD, result, std::vector<std::shared_ptr<IRValue>>{op1, op2});
    
    entryBlock->addInstruction(addInst);
    
    auto retInst = std::make_shared<IRInstruction>(
        IROpcode::RET, nullptr, std::vector<std::shared_ptr<IRValue>>{result});
    
    entryBlock->addInstruction(retInst);
    
    func->addBlock(entryBlock);
    
    module->addFunction(func);
    
    std::string expected = "; ModuleID = 'test_module'\n\n"
                          "define i32 @add(i32 %a, i32 %b) {\n"
                          "entry:\n"
                          "  %result = add %a, %b\n"
                          "  ret %result\n"
                          "}\n\n";
    
    EXPECT_EQ(expected, module->toString());
}

TEST(IRTest, IRBuilderSimpleExpression) {
    // 创建AST
    Lexer lexer("int main() { return 1 + 2; }");
    auto tokens = lexer.scanTokens();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    // 生成IR
    IRBuilder builder("test");
    auto module = builder.build(ast.get());
    
    // 验证IR
    ASSERT_NE(nullptr, module);
    ASSERT_EQ(1, module->getFunctions().size());
    
    auto func = module->getFunctions()[0];
    EXPECT_EQ("main", func->getName());
    EXPECT_EQ(IRType::INT32, func->getReturnType());
    EXPECT_EQ(0, func->getParameters().size());
    
    // 检查IR是否包含加法和返回指令
    std::string ir = module->toString();
    EXPECT_TRUE(ir.find("add") != std::string::npos);
    EXPECT_TRUE(ir.find("ret") != std::string::npos);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 