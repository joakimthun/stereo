#pragma once

#include <vector>

#include "../common/typedef.h"
#include "pe.h"

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

        struct CodedIndexInfo
        {
            u8 size;
            u8 num_bits;
            std::vector<MetadataTable> tables;
        };

        CodedIndexInfo get_coded_index_info(CodedIndexType type, const TableInfo* tables);

    }
}