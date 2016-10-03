#include "execution_engine.h"

#include <stdlib.h>

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
            stack_ = static_cast<StereoObject*>(malloc(1024 * 1024));
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
                    auto method_ref = static_cast<const assemblies::MethodRef*>(current_instruction_->operand);
                    call(method_ref);
                    break;
                }
                case assemblies::Code::RET: {
                    ret();
                    break;
                }
                default:
                    logger_->LogError(L"ExecutionEngine::execute -> Unhandled opcode");
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
            auto method_ref = static_cast<const assemblies::MethodRef*>(current_instruction_->operand);
            if (!try_call_interceptor(method_ref->fullname(), stack_, sp_))
                throw "ExecutionEngine::call -> Unsupported MethodRef";
        }

        void ExecutionEngine::ret()
        {
            call_stack_.pop();
        }

        void ExecutionEngine::ldstr()
        {
            auto inl_str = static_cast<const assemblies::InlineString*>(current_instruction_->operand);
            stack_[++sp_] = StereoObject(&inl_str->value);
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