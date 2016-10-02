#pragma once

#include <string>

#include "../common/typedef.h"
#include "resolution_scope.h"
#include "element_type.h"

namespace stereo {
    namespace assemblies {

        struct TypeRef : IResolutionScope
        {
            TypeRef() {}
            TypeRef(const std::wstring& name, const std::wstring& name_space) : name(name), name_space(name_space) {}

            const IResolutionScope* resolution_scope;
            std::wstring name;
            std::wstring name_space;
        };

        const TypeRef* get_primitive_type(ElementType element_type);

    }
}
