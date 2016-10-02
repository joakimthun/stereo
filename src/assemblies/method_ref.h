#pragma once

#include <vector>
#include <memory>

#include "calling_convention.h"
#include "parameter_def.h"
#include "member_ref.h"

namespace stereo {
    namespace assemblies {

        struct MethodRef : public MemberRef
        {
            MethodRef(const std::wstring& name, const TypeRef* declaring_type) : MemberRef(name, declaring_type) {}

            bool has_this;
            bool explicit_this;
            CallingConvention calling_convention;
            u32 num_params;
            const TypeRef* return_type;
            std::vector<std::unique_ptr<ParameterDef>> parameters;

            inline std::wstring fullname() const override
            {
                std::wstring parameter_str(L"(");
                for (const auto& p : parameters)
                {
                    parameter_str += p->type->fullname();

                    if(p != parameters.back())
                        parameter_str += L", ";
                }

                return declaring_type->fullname() + L"." + name + parameter_str + L")";
            }
        };

    }
}