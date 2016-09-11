#include "assembly.h"

#include "../common/ptr_util.h"
#include "../common/byteswap.h"

namespace stereo {
    namespace assemblies {

        Assembly::Assembly(const pe::PEImage* image)
            :
            image_(image)
        {
        }

        u32 Assembly::get_entry_point()
        {
            auto ept = image_->cli_header.entry_point_token;
            return ept & 0x00ffffff;
        }

        const ModuleDef * Assembly::get_module()
        {
            return module_.get();
        }

        const std::vector<std::unique_ptr<MethodDef>>& Assembly::get_methods()
        {
            return methods_;
        }

        void Assembly::read_module()
        {
            module_ = std::make_unique<ModuleDef>();

            auto table_row_ptr = get_table_row_ptr(pe::MetadataTable::Module, 1);
            table_row_ptr += 2; // Skip the generation column since it's always zero

            auto name_string_index = read_string_index(&table_row_ptr);
            module_->name = read_string(name_string_index);

            // TODO: Read the mvid guid
        }

        void Assembly::read_method(u32 rid)
        {
            auto table_row_ptr = get_table_row_ptr(pe::MetadataTable::Method, rid);
            auto method = std::make_unique<MethodDef>();

            method->rva = common::read32(table_row_ptr);
            table_row_ptr += 4;

            method->impl_attributes = static_cast<MethodImplAttributes>(common::read16(table_row_ptr));
            table_row_ptr += 2;

            method->attributes = static_cast<MethodAttributes>(common::read16(table_row_ptr));
            table_row_ptr += 2;

            auto name_string_index = read_string_index(&table_row_ptr);
            method->name = read_string(name_string_index);

            // TODO: Read signature, parameters etc

            read_method_body(method.get());

            methods_.push_back(std::move(method));
        }

        void Assembly::read_method_body(MethodDef* method)
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

                read_method_body_instructions(method);
            }
            else
            {
                // TODO: Implement fat header reading
            }
        }

        void Assembly::read_method_body_instructions(MethodDef * method)
        {

        }

        std::string Assembly::read_string(u32 index)
        {
            std::string value;

            if (index == 0)
                return value;

            auto string_ptr = image_->heap_strings.data + index;

            return std::string((char*)string_ptr);
        }

        u32 Assembly::read_string_index(u8** index_ptr)
        {
            auto index_size = image_->string_idx_size;

            u32 str_index = 0;
            if (index_size == 2)
            {
                str_index = common::read16(*index_ptr);
                *index_ptr += 2;
            }
            else
            {
                str_index = common::read32(*index_ptr);
                *index_ptr += 4;
            }

            return str_index;
        }

        u8* Assembly::get_table_row_ptr(pe::MetadataTable table_type, u32 rid)
        {
            auto table = image_->tables[static_cast<int>(table_type)];
            if (table.rows == 0)
                return nullptr;

            return const_cast<u8*>(table.base + (table.row_size * (rid - 1)));
        }

        const u8* Assembly::get_method_body_ptr(u32 rva)
        {
            return image_->raw_data + resolve_rva(rva);
        }

        u32 Assembly::resolve_rva(u32 rva)
        {
            auto section = resolve_rva_section(rva);
            return rva + section->raw_data_ptr - section->virtual_address;
        }

        const pe::SectionTable* Assembly::resolve_rva_section(u32 rva)
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