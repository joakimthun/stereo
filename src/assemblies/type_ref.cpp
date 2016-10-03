#include "type_ref.h"

#include <array>

#include "../exceptions/not_implemented_exception.h"

namespace stereo {
    namespace assemblies {

        static std::array<TypeRef, 0xe + 1> builtin_types = {
            TypeRef(L"", L""),
            TypeRef(L"Void", L"System"),
            TypeRef(L"Boolean", L"System"),
            TypeRef(L"Char", L"System"),
            TypeRef(L"SByte", L"System"),
            TypeRef(L"Byte", L"System"),
            TypeRef(L"Int16", L"System"),
            TypeRef(L"UInt16", L"System"),
            TypeRef(L"Int32", L"System"),
            TypeRef(L"UInt32", L"System"),
            TypeRef(L"Int64", L"System"),
            TypeRef(L"UInt64", L"System"),
            TypeRef(L"Single", L"System"),
            TypeRef(L"Double", L"System"),
            TypeRef(L"String", L"System"),
        };

        const TypeRef* get_primitive_type(ElementType element_type)
        {
            switch (element_type)
            {
            case ElementType::Void: 
            case ElementType::Boolean:
            case ElementType::Char:
            case ElementType::I1:
            case ElementType::U1:
            case ElementType::I2:
            case ElementType::U2:
            case ElementType::I4:
            case ElementType::U4:
            case ElementType::I8:
            case ElementType::U8:
            case ElementType::R4:
            case ElementType::R8:
            case ElementType::String:
                return &builtin_types[static_cast<u8>(element_type)];
            default:
                throw exceptions::NotImplementedException(L"get_builtin_type -> unsupported element type");
            }
        }

    }
}
