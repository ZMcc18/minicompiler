#include "ir/ir.h"
#include <sstream>
#include <iomanip>

namespace minicompiler {

std::string irTypeToString(IRType type) {
    switch (type) {
        case IRType::VOID: return "void";
        case IRType::INT32: return "i32";
        case IRType::FLOAT32: return "f32";
        case IRType::POINTER: return "ptr";
        case IRType::LABEL: return "label";
        default: return "unknown";
    }
}

std::string irOpcodeToString(IROpcode opcode) {
    switch (opcode) {
        case IROpcode::ALLOCA: return "alloca";
        case IROpcode::LOAD: return "load";
        case IROpcode::STORE: return "store";
        
        case IROpcode::ADD: return "add";
        case IROpcode::SUB: return "sub";
        case IROpcode::MUL: return "mul";
        case IROpcode::DIV: return "div";
        case IROpcode::MOD: return "mod";
        case IROpcode::NEG: return "neg";
        
        case IROpcode::CMP_EQ: return "cmp_eq";
        case IROpcode::CMP_NE: return "cmp_ne";
        case IROpcode::CMP_LT: return "cmp_lt";
        case IROpcode::CMP_LE: return "cmp_le";
        case IROpcode::CMP_GT: return "cmp_gt";
        case IROpcode::CMP_GE: return "cmp_ge";
        
        case IROpcode::AND: return "and";
        case IROpcode::OR: return "or";
        case IROpcode::NOT: return "not";
        
        case IROpcode::JMP: return "jmp";
        case IROpcode::JMP_IF: return "jmp_if";
        case IROpcode::CALL: return "call";
        case IROpcode::RET: return "ret";
        
        case IROpcode::INT_TO_FLOAT: return "int_to_float";
        case IROpcode::FLOAT_TO_INT: return "float_to_int";
        
        case IROpcode::PHI: return "phi";
        case IROpcode::LABEL: return "label";
        case IROpcode::COMMENT: return "comment";
        
        default: return "unknown";
    }
}

std::string IRIntConstant::toString() const {
    return std::to_string(value_);
}

std::string IRFloatConstant::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value_;
    return oss.str();
}

std::string IRIdentifier::toString() const {
    return "%" + name_;
}

std::string IRLabel::toString() const {
    return name_ + ":";
}

std::string IRInstruction::toString() const {
    std::ostringstream oss;
    
    // 输出结果
    if (result_) {
        oss << result_->toString() << " = ";
    }
    
    // 输出操作码
    oss << irOpcodeToString(opcode_);
    
    // 输出操作数
    if (!operands_.empty()) {
        oss << " ";
        for (size_t i = 0; i < operands_.size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            oss << operands_[i]->toString();
        }
    }
    
    return oss.str();
}

std::string IRBasicBlock::toString() const {
    std::ostringstream oss;
    
    // 输出基本块标签
    oss << name_ << ":\n";
    
    // 输出指令
    for (const auto& instruction : instructions_) {
        oss << "  " << instruction->toString() << "\n";
    }
    
    return oss.str();
}

std::string IRFunction::toString() const {
    std::ostringstream oss;
    
    // 输出函数签名
    oss << "define " << irTypeToString(returnType_) << " @" << name_ << "(";
    
    // 输出参数
    for (size_t i = 0; i < parameters_.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << irTypeToString(parameters_[i].type) << " %" << parameters_[i].name;
    }
    oss << ") {\n";
    
    // 输出基本块
    for (const auto& block : blocks_) {
        oss << block->toString();
    }
    
    oss << "}\n";
    
    return oss.str();
}

std::string IRModule::toString() const {
    std::ostringstream oss;
    
    // 输出模块名
    oss << "; ModuleID = '" << name_ << "'\n\n";
    
    // 输出函数
    for (const auto& function : functions_) {
        oss << function->toString() << "\n";
    }
    
    return oss.str();
}

} // namespace minicompiler 