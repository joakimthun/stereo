#pragma once

#include <string>

#include "stack_impl.h"
#include "stereo_object.h"

#include "../assemblies/operand.h"

namespace stereo {
    namespace runtime {

        class Stack : public StackImpl<StereoObject, 1000>
        {
        public:
            void push_str(const assemblies::InlineString* str);
        };

    }
}