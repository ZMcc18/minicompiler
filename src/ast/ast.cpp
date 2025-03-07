#include "ast/ast.h"

namespace minicompiler {

void IntegerLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void FloatLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void StringLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void VariableExpression::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void BinaryExpression::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void UnaryExpression::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void CallExpression::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void ExpressionStatement::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void VarDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void BlockStatement::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void IfStatement::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void WhileStatement::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void ReturnStatement::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void FunctionDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

void Program::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}

} // namespace minicompiler 