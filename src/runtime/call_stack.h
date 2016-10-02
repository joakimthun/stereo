#pragma once

#include "stack_impl.h"
#include "../assemblies/method_def.h"

namespace stereo {
    namespace runtime {

        struct StackFrame
        {
            StackFrame() : method(nullptr), start_index(0) {}
            StackFrame(const assemblies::MethodDef* method, u32 start_index) : method(method), start_index(0) {}

            const assemblies::MethodDef* method;
            u32 start_index;
        };

        class CallStack : public StackImpl<StackFrame, 1000>
        {
        };

    }
}