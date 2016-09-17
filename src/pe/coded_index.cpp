#pragma once

#include <algorithm>

#include "coded_index.h"

namespace stereo {
    namespace pe {

        CodedIndexInfo get_coded_index_info(CodedIndexType type, const TableInfo* tables)
        {
            // II.24.2.6 #~ stream 

            CodedIndexInfo info;

            switch (type) {
            case CodedIndexType::TypeDefOrRef:
                info.num_bits = 2;
                info.tables = { MetadataTable::TypeDef, MetadataTable::TypeRef, MetadataTable::TypeSpec };
                break;
            case CodedIndexType::HasConstant:
                info.num_bits = 2;
                info.tables = { MetadataTable::Field, MetadataTable::Param, MetadataTable::Property };
                break;
            case CodedIndexType::HasCustomAttribute:
                info.num_bits = 5;
                info.tables = {
                    MetadataTable::Method, MetadataTable::Field, MetadataTable::TypeRef, MetadataTable::TypeDef, MetadataTable::Param, MetadataTable::InterfaceImpl, MetadataTable::MemberRef,
                    MetadataTable::Module, MetadataTable::DeclSecurity, MetadataTable::Property, MetadataTable::Event, MetadataTable::StandAloneSig, MetadataTable::ModuleRef,
                    MetadataTable::TypeSpec, MetadataTable::Assembly, MetadataTable::AssemblyRef, MetadataTable::File, MetadataTable::ExportedType,
                    MetadataTable::ManifestResource, MetadataTable::GenericParam, MetadataTable::GenericParamConstraint, MetadataTable::MethodSpec,
                };
                break;
            case CodedIndexType::HasFieldMarshal:
                info.num_bits = 1;
                info.tables = { MetadataTable::Field, MetadataTable::Param };
                break;
            case CodedIndexType::HasDeclSecurity:
                info.num_bits = 2;
                info.tables = { MetadataTable::TypeDef, MetadataTable::Method, MetadataTable::Assembly };
                break;
            case CodedIndexType::MemberRefParent:
                info.num_bits = 3;
                info.tables = { MetadataTable::TypeDef, MetadataTable::TypeRef, MetadataTable::ModuleRef, MetadataTable::Method, MetadataTable::TypeSpec };
                break;
            case CodedIndexType::HasSemantics:
                info.num_bits = 1;
                info.tables = { MetadataTable::Event, MetadataTable::Property };
                break;
            case CodedIndexType::MethodDefOrRef:
                info.num_bits = 1;
                info.tables = { MetadataTable::Method, MetadataTable::MemberRef };
                break;
            case CodedIndexType::MemberForwarded:
                info.num_bits = 1;
                info.tables = { MetadataTable::Field, MetadataTable::Method };
                break;
            case CodedIndexType::Implementation:
                info.num_bits = 2;
                info.tables = { MetadataTable::File, MetadataTable::AssemblyRef, MetadataTable::ExportedType };
                break;
            case CodedIndexType::CustomAttributeType:
                info.num_bits = 3;
                info.tables = { MetadataTable::Method, MetadataTable::MemberRef };
                break;
            case CodedIndexType::ResolutionScope:
                info.num_bits = 2;
                info.tables = { MetadataTable::Module, MetadataTable::ModuleRef, MetadataTable::AssemblyRef, MetadataTable::TypeRef };
                break;
            case CodedIndexType::TypeOrMethodDef:
                info.num_bits = 1;
                info.tables = { MetadataTable::TypeDef, MetadataTable::Method };
                break;
            default:
                return CodedIndexInfo{ 0 };
            }

            u32 max = 0;
            for (const auto& t : info.tables)
            {
                max = std::max(tables[static_cast<u8>(t)].rows, max);
            }

            // If the max number of table rows for any one of the possible table pointers are greater than the biggest number that can fit into the remaining bits the index needs 4 bytes instead of 2
            info.size = max < static_cast<u32>((1 << (16 - info.num_bits))) ? 2 : 4;

            return info;
        }

    }
}