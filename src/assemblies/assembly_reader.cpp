#include "assembly_reader.h"

#include "../common/ptr_util.h"
#include "opcodes.h"
#include "../pe/coded_index.h"
#include "../pe/pe_image_reader.h"

namespace stereo {
    namespace assemblies {

        AssemblyReader::AssemblyReader(const char* file_path, Assembly* assembly)
            :
            assembly_(assembly),
            logger_(std::make_unique<logging::ConsoleLogger>()),
            image_(pe::PEImageReader::load_image(file_path))
        {
        }

        u32 AssemblyReader::get_entry_point()
        {
            auto ept = pe::MetadataToken(image_->cli_header.entry_point_token);
            return ept.rid();
        }

        u32 AssemblyReader::get_num_entries(pe::MetadataTable table)
        {
            auto table_info = image_->tables[static_cast<int>(table)];
            return table_info.rows;
        }

        std::unique_ptr<ModuleDef> AssemblyReader::read_module_def()
        {
            auto module = std::make_unique<ModuleDef>();

            auto table_row_ptr = get_table_row_ptr(pe::MetadataTable::Module, 1);
            table_row_ptr += 2; // Skip the generation column since it's always zero

            module->name = read_string(&table_row_ptr);

            // TODO: Read the mvid guid

            return module;
        }

        std::unique_ptr<MethodDef> AssemblyReader::read_method_def(u32 rid)
        {
            auto table_row_ptr = get_table_row_ptr(pe::MetadataTable::Method, rid);
            auto method_def = std::make_unique<MethodDef>();

            // RVA (a 4-byte constant) 
            method_def->rva = ptrutil::read32(&table_row_ptr);

            // ImplFlags (a 2-byte bitmask of type MethodImplAttributes, §II.23.1.10) 
            method_def->impl_attributes = static_cast<MethodImplAttributes>(ptrutil::read16(&table_row_ptr));

            // Flags (a 2-byte bitmask of type MethodAttributes, §II.23.1.10) 
            method_def->attributes = static_cast<MethodAttributes>(ptrutil::read16(&table_row_ptr));

            // Name (an index into the String heap) 
            method_def->name = read_string(&table_row_ptr);

            // TODO: Read signature, parameters etc

            read_method_body(method_def.get());

            return method_def;
        }

        std::unique_ptr<MemberRef> AssemblyReader::read_member_ref(u32 rid)
        {
            auto table_row_ptr = get_table_row_ptr(pe::MetadataTable::MemberRef, rid);

            // Class (an index into the MethodDef, ModuleRef, TypeDef, TypeRef, or TypeSpec tables; more precisely, a MemberRefParent(§II.24.2.6) coded index)
            auto token = read_metadata_token(&table_row_ptr, pe::CodedIndexType::MemberRefParent);

            // Name (an index into the String heap) 
            auto name = read_string(&table_row_ptr);

            // Signature (an index into the Blob heap) 
            auto signature = read_blob_index(&table_row_ptr);

            // TODO: Implement all token type cases
            if (token.type() == pe::MetadataTokenType::TypeRef)
                read_type_ref(token.rid());

            return std::make_unique<MemberRef>(name);
        }

        void AssemblyReader::read_method_body(MethodDef* method)
        {
            method->body = std::make_unique<MethodBody>();
            auto method_body_ptr = get_method_body_ptr(method->rva);

            // The two least significant bits of the first byte of the method header indicate what type of header is present.
            auto header_flag = *method_body_ptr;
            method_body_ptr++;

            auto tiny_header = (header_flag & 0x3) == CorILMethod_TinyFormat;
            if (tiny_header)
            {
                // Tiny headers use a 6-bit length encoding
                method->body->code_size = header_flag >> 2;
                // The operand stack shall be no bigger than 8 entries
                method->body->max_stack_size = 8;

                read_method_body_instructions(method, method_body_ptr);
            }
            else
            {
                // TODO: Implement fat header reading
            }
        }

        void AssemblyReader::read_method_body_instructions(MethodDef * method, u8* method_body_ptr)
        {
            auto opcode = *method_body_ptr;
            auto read = true;

            while (read)
            {
                switch (static_cast<Opcodes>(opcode))
                {
                case Opcodes::Ldstr: {
                    logger_->LogInfo(L"ldstr");
                    method_body_ptr++;

                    auto str_token = read_metadata_token(&method_body_ptr);
                    auto is_str_token = str_token.type() == pe::MetadataTokenType::String;
                    auto str = read_us_string(str_token.rid());
                    logger_->LogInfo(str);
                    break;
                }
                case Opcodes::Call: {
                    logger_->LogInfo(L"call");
                    method_body_ptr++;

                    auto token = read_metadata_token(&method_body_ptr);

                    auto rid = token.rid();
                    auto type = token.type();

                    read_member_ref(rid);

                    // TODO: Break on ret
                    read = false;
                    break;
                }
                default:
                    break;
                }

                opcode = *method_body_ptr;
            }
        }

        std::unique_ptr<TypeRef> AssemblyReader::read_type_ref(u32 rid)
        {
            auto table_row_ptr = get_table_row_ptr(pe::MetadataTable::TypeRef, rid);

            //ResolutionScope (an index into a Module, ModuleRef, AssemblyRef or TypeRef table, or null; more precisely, a ResolutionScope(§II.24.2.6) coded index)
            auto res_scope = read_metadata_token(&table_row_ptr, pe::CodedIndexType::ResolutionScope);

            // TypeName(an index into the String heap)
            auto name = read_string(&table_row_ptr);

            // TypeNamespace(an index into the String heap)
            auto name_space = read_string(&table_row_ptr);

            if (res_scope.type() == pe::MetadataTokenType::AssemblyRef)
            {

            }
            else
            {
                throw "read_type_ref -> unsupported ResolutionScope type";
            }

            return std::make_unique<TypeRef>(name, name_space);
        }

        std::wstring AssemblyReader::read_us_string(u32 index)
        {
            std::wstring value;

            if (index == 0)
                return value;

            auto str_ptr = image_->heap_us.data + index;

            // TODO: Implement according to II.24.2.4 #US and #Blob heaps
            auto length = *str_ptr & 0xfffffffe;
            str_ptr++;

            return strutil::to_utf16wstr(str_ptr, length);
        }

        std::wstring AssemblyReader::read_string(u8** index_ptr)
        {
            auto index = read_string_index(index_ptr);

            std::wstring value;

            if (index == 0)
                return value;

            auto string_ptr = image_->heap_strings.data + index;

            auto utf8_str = std::string(reinterpret_cast<const char*>(string_ptr));
            return strutil::utf8str_to_utf16wstr(utf8_str);
        }

        u32 AssemblyReader::read_string_index(u8** index_ptr)
        {
            auto index_size = image_->string_idx_size;

            u32 str_index = 0;
            if (index_size == 2)
            {
                str_index = ptrutil::read16(index_ptr);
            }
            else
            {
                str_index = ptrutil::read32(index_ptr);
            }

            return str_index;
        }

        u32 AssemblyReader::read_blob_index(u8 ** index_ptr)
        {
            if (image_->blob_idx_size == 2)
                return ptrutil::read16(index_ptr);

            return ptrutil::read32(index_ptr);
        }

        pe::MetadataToken AssemblyReader::read_metadata_token(u8** ptr)
        {
            auto value = ptrutil::read32(ptr);
            return pe::MetadataToken(value);
        }

        pe::MetadataToken AssemblyReader::read_metadata_token(u8** ptr, pe::CodedIndexType index_type)
        {
            auto coded_index_info = pe::get_coded_index_info(index_type, image_->tables);
            u32 token;
            if (coded_index_info.size == 2)
                token = ptrutil::read16(ptr);
            else
                token = ptrutil::read32(ptr);

            return pe::get_metadata_token_from_coded_index(coded_index_info, token);
        }

        u8* AssemblyReader::get_table_row_ptr(pe::MetadataTable table_type, u32 rid)
        {
            auto table = image_->tables[static_cast<int>(table_type)];
            if (table.rows == 0)
                return nullptr;

            return const_cast<u8*>(table.base + (table.row_size * (rid - 1)));
        }

        u8* AssemblyReader::get_method_body_ptr(u32 rva)
        {
            return const_cast<u8*>(image_->raw_data) + resolve_rva(rva);
        }

        u32 AssemblyReader::resolve_rva(u32 rva)
        {
            auto section = resolve_rva_section(rva);
            return rva + section->raw_data_ptr - section->virtual_address;
        }

        const pe::SectionTable* AssemblyReader::resolve_rva_section(u32 rva)
        {
            for (auto& s : image_->cli_section_tables)
            {
                if (rva >= s->virtual_address && rva < s->virtual_address + s->raw_data_size)
                    return s.get();
            }

            return nullptr;
        }
    }
}