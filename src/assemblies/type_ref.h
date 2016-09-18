#pragma once

#include <string>

#include "../common/typedef.h"

namespace stereo {
    namespace assemblies {

        struct TypeRef
        {
            TypeRef(const std::wstring& name, const std::wstring& name_space) : name(name), name_space(name_space) {}

            std::wstring name;
            std::wstring name_space;
        };

    }
}
