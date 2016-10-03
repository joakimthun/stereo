#pragma once

#include <algorithm>

#include "coded_index.h"
#include "../exceptions/not_implemented_exception.h"

namespace stereo {
    namespace pe {

        CodedIndexInfo get_coded_index_info(CodedIndexType type, const TableInfo* tables)
        {
            // II.24.2.6 #~ stream

            CodedIndexInfo info;

            switch (type) 
            {
            case CodedIndexType::TypeDefOrRef:
                info.num_bits = 2;
                info.table_mask = 3;
                info.tables = { MetadataTable::TypeDef, MetadataTable::TypeRef, MetadataTable::TypeSpec };
                break;
            case CodedIndexType::HasConstant:
                info.num_bits = 2;
                info.table_mask = 3;
                info.tables = { MetadataTable::Field, MetadataTable::Param, MetadataTable::Property };
                break;
            case CodedIndexType::HasCustomAttribute:
                info.num_bits = 5;
                info.table_mask = 31; // 0b11111
                info.tables = {
                    MetadataTable::Method, MetadataTable::Field, MetadataTable::TypeRef, MetadataTable::TypeDef, MetadataTable::Param, MetadataTable::InterfaceImpl, MetadataTable::MemberRef,
                    MetadataTable::Module, MetadataTable::DeclSecurity, MetadataTable::Property, MetadataTable::Event, MetadataTable::StandAloneSig, MetadataTable::ModuleRef,
                    MetadataTable::TypeSpec, MetadataTable::Assembly, MetadataTable::AssemblyRef, MetadataTable::File, MetadataTable::ExportedType,
                    MetadataTable::ManifestResource, MetadataTable::GenericParam, MetadataTable::GenericParamConstraint, MetadataTable::MethodSpec,
                };
                break;
            case CodedIndexType::HasFieldMarshal:
                info.num_bits = 1;
                info.table_mask = 1;
                info.tables = { MetadataTable::Field, MetadataTable::Param };
                break;
            case CodedIndexType::HasDeclSecurity:
                info.num_bits = 2;
                info.table_mask = 3;
                info.tables = { MetadataTable::TypeDef, MetadataTable::Method, MetadataTable::Assembly };
                break;
            case CodedIndexType::MemberRefParent:
                info.num_bits = 3;
                info.table_mask = 7;
                info.tables = { MetadataTable::TypeDef, MetadataTable::TypeRef, MetadataTable::ModuleRef, MetadataTable::Method, MetadataTable::TypeSpec };
                break;
            case CodedIndexType::HasSemantics:
                info.num_bits = 1;
                info.table_mask = 1;
                info.tables = { MetadataTable::Event, MetadataTable::Property };
                break;
            case CodedIndexType::MethodDefOrRef:
                info.num_bits = 1;
                info.table_mask = 1;
                info.tables = { MetadataTable::Method, MetadataTable::MemberRef };
                break;
            case CodedIndexType::MemberForwarded:
                info.num_bits = 1;
                info.table_mask = 1;
                info.tables = { MetadataTable::Field, MetadataTable::Method };
                break;
            case CodedIndexType::Implementation:
                info.num_bits = 2;
                info.table_mask = 3;
                info.tables = { MetadataTable::File, MetadataTable::AssemblyRef, MetadataTable::ExportedType };
                break;
            case CodedIndexType::CustomAttributeType:
                info.num_bits = 3;
                info.table_mask = 7;
                info.tables = { MetadataTable::Method, MetadataTable::MemberRef };
                break;
            case CodedIndexType::ResolutionScope:
                info.num_bits = 2;
                info.table_mask = 3;
                info.tables = { MetadataTable::Module, MetadataTable::ModuleRef, MetadataTable::AssemblyRef, MetadataTable::TypeRef };
                break;
            case CodedIndexType::TypeOrMethodDef:
                info.num_bits = 1;
                info.table_mask = 1;
                info.tables = { MetadataTable::TypeDef, MetadataTable::Method };
                break;
            default:
                throw exceptions::NotImplementedException(L"get_coded_index_info -> unsupported coded index type");
            }

            u32 max = 0;
            for (const auto& t : info.tables)
            {
                max = std::max(tables[static_cast<u8>(t)].rows, max);
            }

            // If the max number of table rows for any one of the possible table pointers are greater than the biggest number that can fit into the remaining bits the index needs 4 bytes instead of 2
            info.size = max < static_cast<u32>((1 << (16 - info.num_bits))) ? 2 : 4;
            info.type = type;

            return info;
        }

        MetadataToken get_metadata_token_from_coded_index(const CodedIndexInfo& index_info, u32 token)
        {
            // II.24.2.6 #~stream
            // For example, the Parent column of the Constant table indexes a row in the Field, Param, or Property tables.The actual table is encoded into the low 2 bits of the number, 
            // using the values : 0 = > Field, 1 = > Param, 2 = > Property.The remaining bits hold the actual row number being indexed.

            // Shift away the bits that encoded the table
            auto rid = token >> index_info.num_bits;

            switch (index_info.type)
            {
            case CodedIndexType::TypeDefOrRef:
            {
                switch (token & index_info.table_mask)
                {
                case 0:
                    return MetadataToken(MetadataTokenType::TypeDef, rid);
                case 1:
                    return MetadataToken(MetadataTokenType::TypeRef, rid);
                case 2:
                    return MetadataToken(MetadataTokenType::TypeSpec, rid);
                default:
                    break;
                }
            }
            case CodedIndexType::HasConstant:
            {
                switch (token & index_info.table_mask)
                {
                case 0:
                    return MetadataToken(MetadataTokenType::Field, rid);
                case 1:
                    return MetadataToken(MetadataTokenType::Param, rid);
                case 2:
                    return MetadataToken(MetadataTokenType::Property, rid);
                default:
                    break;
                }
            }
            case CodedIndexType::HasCustomAttribute:
            {
                switch (token & index_info.table_mask)
                {
                case 0:
                    return MetadataToken(MetadataTokenType::Method, rid);
                case 1:
                    return MetadataToken(MetadataTokenType::Field, rid);
                case 2:
                    return MetadataToken(MetadataTokenType::TypeRef, rid);
                case 3:
                    return MetadataToken(MetadataTokenType::TypeDef, rid);
                case 4:
                    return MetadataToken(MetadataTokenType::Param, rid);
                case 5:
                    return MetadataToken(MetadataTokenType::InterfaceImpl, rid);
                case 6:
                    return MetadataToken(MetadataTokenType::MemberRef, rid);
                case 7:
                    return MetadataToken(MetadataTokenType::Module, rid);
                case 8:
                    return MetadataToken(MetadataTokenType::Permission, rid);
                case 9:
                    return MetadataToken(MetadataTokenType::Property, rid);
                case 10:
                    return MetadataToken(MetadataTokenType::Event, rid);
                case 11:
                    return MetadataToken(MetadataTokenType::Signature, rid);
                case 12:
                    return MetadataToken(MetadataTokenType::ModuleRef, rid);
                case 13:
                    return MetadataToken(MetadataTokenType::TypeSpec, rid);
                case 14:
                    return MetadataToken(MetadataTokenType::Assembly, rid);
                case 15:
                    return MetadataToken(MetadataTokenType::AssemblyRef, rid);
                case 16:
                    return MetadataToken(MetadataTokenType::File, rid);
                case 17:
                    return MetadataToken(MetadataTokenType::ExportedType, rid);
                case 18:
                    return MetadataToken(MetadataTokenType::ManifestResource, rid);
                case 19:
                    return MetadataToken(MetadataTokenType::GenericParam, rid);
                case 20:
                    return MetadataToken(MetadataTokenType::GenericParamConstraint, rid);
                case 21:
                    return MetadataToken(MetadataTokenType::MethodSpec, rid);
                default:
                    break;
                }
            }
            case CodedIndexType::HasFieldMarshal:
            {
                switch (token & index_info.table_mask)
                {
                case 0:
                    return MetadataToken(MetadataTokenType::Field, rid);
                case 1:
                    return MetadataToken(MetadataTokenType::Param, rid);
                default:
                    break;
                }
            }
            case CodedIndexType::HasDeclSecurity:
            {
                switch (token & index_info.table_mask)
                {
                case 0:
                    return MetadataToken(MetadataTokenType::TypeDef, rid);
                case 1:
                    return MetadataToken(MetadataTokenType::Method, rid);
                case 2:
                    return MetadataToken(MetadataTokenType::Assembly, rid);
                default:
                    break;
                }
            }
            case CodedIndexType::MemberRefParent:
            {
                switch (token & index_info.table_mask)
                {
                case 0:
                    return MetadataToken(MetadataTokenType::TypeDef, rid);
                case 1:
                    return MetadataToken(MetadataTokenType::TypeRef, rid);
                case 2:
                    return MetadataToken(MetadataTokenType::ModuleRef, rid);
                case 3:
                    return MetadataToken(MetadataTokenType::Method, rid);
                case 4:
                    return MetadataToken(MetadataTokenType::TypeSpec, rid);
                default:
                    break;
                }
            }
            case CodedIndexType::HasSemantics:
            {
                switch (token & index_info.table_mask)
                {
                case 0:
                    return MetadataToken(MetadataTokenType::Event, rid);
                case 1:
                    return MetadataToken(MetadataTokenType::Property, rid);
                default:
                    break;
                }
            }
            case CodedIndexType::MethodDefOrRef:
            {
                switch (token & index_info.table_mask)
                {
                case 0:
                    return MetadataToken(MetadataTokenType::Method, rid);
                case 1:
                    return MetadataToken(MetadataTokenType::MemberRef, rid);
                default:
                    break;
                }
            }
            case CodedIndexType::MemberForwarded:
            {
                switch (token & index_info.table_mask)
                {
                case 0:
                    return MetadataToken(MetadataTokenType::Field, rid);
                case 1:
                    return MetadataToken(MetadataTokenType::Method, rid);
                default:
                    break;
                }
            }
            case CodedIndexType::Implementation:
            {
                switch (token & index_info.table_mask)
                {
                case 0:
                    return MetadataToken(MetadataTokenType::File, rid);
                case 1:
                    return MetadataToken(MetadataTokenType::AssemblyRef, rid);
                case 2:
                    return MetadataToken(MetadataTokenType::ExportedType, rid);
                default:
                    break;
                }
            }
            case CodedIndexType::CustomAttributeType:
            {
                switch (token & index_info.table_mask)
                {
                case 2:
                    return MetadataToken(MetadataTokenType::Method, rid);
                case 3:
                    return MetadataToken(MetadataTokenType::MemberRef, rid);
                default:
                    break;
                }
            }
            case CodedIndexType::ResolutionScope:
            {
                switch (token & index_info.table_mask)
                {
                case 0:
                    return MetadataToken(MetadataTokenType::Module, rid);
                case 1:
                    return MetadataToken(MetadataTokenType::ModuleRef, rid);
                case 2:
                    return MetadataToken(MetadataTokenType::AssemblyRef, rid);
                case 3:
                    return MetadataToken(MetadataTokenType::TypeRef, rid);
                default:
                    break;
                }
            }
            case CodedIndexType::TypeOrMethodDef:
            {
                switch (token & index_info.table_mask)
                {
                case 0:
                    return MetadataToken(MetadataTokenType::TypeDef, rid);
                case 1:
                    return MetadataToken(MetadataTokenType::Method, rid);
                default: break;
                }
            }
            default:
                break;
            }

            throw exceptions::NotImplementedException(L"get_metadata_token_from_coded_index -> unsupported CodedIndexType");
        }

    }
}