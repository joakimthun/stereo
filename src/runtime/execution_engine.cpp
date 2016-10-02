#include "execution_engine.h"

namespace stereo {
    namespace runtime {

        ExecutionEngine::ExecutionEngine(assemblies::Assembly* assembly) 
            : 
            logger_(std::make_unique<logging::ConsoleLogger>()), 
            assembly_(assembly), 
            ip_(0),
            sp_(0)
        {
        }

        ExecutionEngine::~ExecutionEngine() {}

        void ExecutionEngine::execute()
        {
            auto ep = assembly_->get_entry_point();
            auto main = assembly_->get_method_def(ep);
            execute(main);
        }

        void ExecutionEngine::execute(const assemblies::MethodDef* method)
        {
            for (const auto& instruction : method->body->instructions)
            {
                switch (instruction->code.code)
                {
                case assemblies::Code::LDSTR: {
                    auto str = static_cast<const assemblies::InlineString*>(instruction->operand);
                    logger_->LogInfo(instruction->code.name + L": " + str->value);

                    break;
                }
                case assemblies::Code::CALL: {
                    auto method_ref = static_cast<const assemblies::MethodRef*>(instruction->operand);
                    logger_->LogInfo(instruction->code.name + L": " + method_ref->fullname());

                    break;
                }
                case assemblies::Code::RET: {
                    logger_->LogInfo(instruction->code.name);
                    break;
                }
                default:
                    logger_->LogError(L"ExecutionEngine::execute -> Unhandled opcode");
                    break;
                }
            }
        }
    }
}