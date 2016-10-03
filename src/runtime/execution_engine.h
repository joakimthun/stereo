#pragma once

#include <memory>
#include <string>

#include "../common/console_logger.h"
#include "../common/typedef.h"
#include "stereo_object.h"
#include "call_stack.h"
#include "../assemblies/assembly.h"
#include "../assemblies/method_def.h"
#include "../assemblies/method_ref.h"
#include "../assemblies/member_ref.h"

#define MAX_STACK 1000

namespace stereo {
    namespace runtime {

        class ExecutionEngine
        {
        public:
            ExecutionEngine(assemblies::Assembly* assembly);
            ~ExecutionEngine();

            void execute();

        private:
            void cycle();
            void call(const assemblies::MethodDef* method);
            void call(const assemblies::MethodRef* method);
            void ret();
            void ldstr();
            void set_current_instruction();

            std::unique_ptr<logging::ILogger> logger_;
            assemblies::Assembly* assembly_;
            CallStack call_stack_;
            u32 sp_;
            StereoObject* stack_;
            u32 ip_;
            const assemblies::Instruction* current_instruction_;

        };
    }
}