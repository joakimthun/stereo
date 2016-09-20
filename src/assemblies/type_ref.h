#pragma once

#include <string>

#include "../common/typedef.h"
#include "resolution_scope.h"

namespace stereo {
    namespace assemblies {

        struct TypeRef : IResolutionScope
        {
            const IResolutionScope* resolution_scope;
            std::wstring name;
            std::wstring name_space;
        };

    }
}
