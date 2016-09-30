#pragma once

#include <string>

#include "../common/typedef.h"

namespace stereo {
    namespace assemblies {

        struct IOperand;
        struct TypeRef;

        struct MemberRef : public IOperand
        {
            std::wstring name;
            const TypeRef* type_ref;
        };

    }
}
