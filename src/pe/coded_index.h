#pragma once

#include "../common/typedef.h"

namespace stereo {
    namespace pe {

        // The enum value represents the number of bits needed to encode the index
        enum class CodedIndexType : u8
        {
            TypeDefOrRef,
            HasConstant,
            HasCustomAttribute,
            HasFieldMarshal,
            HasDeclSecurity,
            MemberRefParent,
            HasSemantics,
            MethodDefOrRef,
            MemberForwarded,
            Implementation,
            CustomAttributeType,
            ResolutionScope,
            TypeOrMethodDef
        };

        inline u8 get_coded_index_size_in_bits(CodedIndexType type)
        {
            switch (type)
            {
            case CodedIndexType::HasFieldMarshal:
            case CodedIndexType::HasSemantics:
            case CodedIndexType::MethodDefOrRef:
            case CodedIndexType::MemberForwarded:
            case CodedIndexType::TypeOrMethodDef: return 1;

            case CodedIndexType::TypeDefOrRef:
            case CodedIndexType::HasConstant:
            case CodedIndexType::HasDeclSecurity:
            case CodedIndexType::Implementation:
            case CodedIndexType::ResolutionScope:

            case CodedIndexType::MemberRefParent:
            case CodedIndexType::CustomAttributeType: return 3;

            case CodedIndexType::HasCustomAttribute: return 5;
            default:
                return 0;
            }
        }

    }
}