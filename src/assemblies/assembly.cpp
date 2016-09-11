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

            //method->name = 
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
    }
}