#include "stack.h"

namespace stereo {
    namespace runtime {

        void Stack::push_str(const assemblies::InlineString* str)
        {
            push(StereoObject(&str->value));
        }

    }
}
