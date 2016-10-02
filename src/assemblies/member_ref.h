#pragma once

#include <string>

#include "../common/typedef.h"
#include "type_ref.h"

namespace stereo {
    namespace assemblies {

        struct IOperand;

        struct MemberRef : public IOperand
        {
            MemberRef(const std::wstring& name, const TypeRef* declaring_type) : name(name), declaring_type(declaring_type) {}

            std::wstring name;
            const TypeRef* declaring_type;

            inline std::wstring fullname() const
            {
                return declaring_type->name_space + L"." + declaring_type->name + L"." + name;
            }
        };

    }
}
