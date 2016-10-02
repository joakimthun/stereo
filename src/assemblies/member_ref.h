#pragma once

#include <string>

#include "../common/typedef.h"

namespace stereo {
    namespace assemblies {

        struct IOperand;
        struct TypeRef;

        struct MemberRef : public IOperand
        {
            MemberRef(const std::wstring& name, const TypeRef* declaring_type) : name(name), declaring_type(declaring_type) {}

            std::wstring name;
            const TypeRef* declaring_type;
        };

    }
}
