#pragma once

namespace stereo {
    namespace assemblies {

        struct TypeRef;

        struct ParameterDef
        {
            ParameterDef(const TypeRef* type) : type(type) {}
            const TypeRef* type;
        };

    }
}
