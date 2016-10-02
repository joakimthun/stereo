#pragma once

#include <vector>

#include "calling_convention.h"
#include "parameter_def.h"
#include "member_ref.h"

namespace stereo {
    namespace assemblies {

        struct TypeRef;

        struct MethodRef : public MemberRef
        {
            MethodRef(const std::wstring& name, const TypeRef* declaring_type) : MemberRef(name, declaring_type) {}

            bool has_this;
            bool explicit_this;
            CallingConvention calling_convention;
            u32 num_params;
            const TypeRef* return_type;
            std::vector<std::unique_ptr<ParameterDef>> parameters;
        };

    }
}