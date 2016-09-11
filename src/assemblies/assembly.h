#pragma once

#include <vector>
#include <memory>

#include "../common/typedef.h"
#include "../pe/pe.h"
#include "../pe/pe_image.h"
#include "method_def.h"
#include "module_def.h"

namespace stereo {
    namespace assemblies {

        class Assembly 
        {
        public: 
            Assembly(const pe::PEImage* image);

            u32 get_entry_point();
            const ModuleDef* get_module();

            void read_module();
            void read_method(u32 rid);

        private:
            void read_method_body(MethodDef* method);
            void read_method_body_instructions(MethodDef* method);
            std::string read_string(u32 index);
            u32 read_string_index(u8** index_ptr);
            u8* get_table_row_ptr(pe::MetadataTable table_type, u32 rid);
            const u8* get_method_body_ptr(u32 rva);
            u32 resolve_rva(u32 rva);
            const pe::SectionTable* resolve_rva_section(u32 rva);

            const pe::PEImage* image_;
            std::unique_ptr<ModuleDef> module_;
            std::vector<std::unique_ptr<MethodDef>> methods_;
        };

    }
}