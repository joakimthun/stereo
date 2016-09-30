#pragma once

#include <memory>

#include "../common/console_logger.h"
#include "../common/typedef.h"
#include "../assemblies/assembly.h"
#include "../assemblies/method_def.h"
#include "../assemblies/member_ref.h"

namespace stereo {
    namespace runtime {

        class ExecutionEngine
        {
        public:
            ExecutionEngine(assemblies::Assembly* assembly);
            ~ExecutionEngine();

            void execute();

        private:
            void execute(const assemblies::MethodDef* method);

            std::unique_ptr<logging::ILogger> logger_;
            assemblies::Assembly* assembly_;
            u64 sp_;
        };
    }
}