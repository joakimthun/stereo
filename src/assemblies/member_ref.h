#pragma once

#include <string>

#include "../common/typedef.h"

namespace stereo {
    namespace assemblies {

        struct MemberRef
        {
            MemberRef(const std::wstring& name) : name(name) {}

            std::wstring name;
        };

    }
}
