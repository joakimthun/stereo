#include "execution_engine.h"

#include <stdlib.h>

#include "../exceptions/runtime_exception.h"
#include "method_interceptors.h"

namespace stereo {
    namespace runtime {

        ExecutionEngine::ExecutionEngine(assemblies::Assembly* assembly) 
            : 
            logger_(std::make_unique<logging::ConsoleLogger>()), 
            assembly_(assembly), 
            ip_(0),
            sp_(0)
        {
            stack_ = static_cast<StereoObject*>(calloc(1024, 1024));
        }

        ExecutionEngine::~ExecutionEngine() 
        {
            free(stack_);
        }

        void ExecutionEngine::execute()
        {
            auto ep = assembly_->get_entry_point();
            auto main = assembly_->get_method_def(ep);
            call(main);
            cycle();
        }

        void ExecutionEngine::cycle()
        {
            while (1)
            {
                set_current_instruction();
                if (current_instruction_ == nullptr)
                    return;

                switch (current_instruction_->code.code)
                {
                case assemblies::Code::LDSTR: {
                    ldstr();
                    break;
                }
                case assemblies::Code::CALL: {
                    auto method_ref = static_cast<const assemblies::MethodRef*>(current_operand());
                    call(method_ref);
                    break;
                }
                case assemblies::Code::RET: {
                    ret();
                    break;
                }
                default:
                    logger_->LogError(L"ExecutionEngine::execute -> Unhandled opcode: " + current_instruction_->code.name);
                    break;
                }

                ip_++;
            }
        }

        void ExecutionEngine::call(const assemblies::MethodDef* method)
        {
            call_stack_.push(StackFrame(method, sp_));
        }

        void ExecutionEngine::call(const assemblies::MethodRef* method)
        {
            auto method_ref = static_cast<const assemblies::MethodRef*>(current_operand());
            if (!try_call_interceptor(method_ref->fullname(), stack_, sp_))
                throw exceptions::RuntimeException(L"ExecutionEngine::call -> Unsupported MethodRef: " + method_ref->fullname());
        }

        void ExecutionEngine::ret()
        {
            call_stack_.pop();
        }

        void ExecutionEngine::ldstr()
        {
            auto inl_str = static_cast<const assemblies::InlineString*>(current_operand());
            stack_[++sp_] = StereoObject(&inl_str->value);
        }

        const assemblies::IOperand* ExecutionEngine::current_operand()
        {
            if (current_instruction_ == nullptr || current_instruction_->ptr_type == assemblies::InstructionOperandPtrType::None)
                return nullptr;

            if (current_instruction_->ptr_type == assemblies::InstructionOperandPtrType::Ptr)
                return static_cast<const assemblies::PtrInstruction*>(current_instruction_)->operand;

            if (current_instruction_->ptr_type == assemblies::InstructionOperandPtrType::UniquePtr)
                return static_cast<const assemblies::UniquePtrInstruction*>(current_instruction_)->operand.get();

            throw exceptions::RuntimeException(L"ExecutionEngine::current_operand -> Unkown InstructionOperandPtrType");
        }

        void ExecutionEngine::set_current_instruction()
        {
            auto call_stack_top = call_stack_.top();
            if (call_stack_top == nullptr)
            {
                current_instruction_ = nullptr;
                return;
            }

            current_instruction_ = call_stack_top->method->body->get_instruction(ip_);
        }
    }
}