#pragma once

#include "string"

namespace stereo {
    namespace assemblies {

        struct IOperand {};

        struct InlineString : public IOperand
        {
            InlineString(const std::wstring& value) : value(value) {}
            std::wstring value;
        };
    }
}