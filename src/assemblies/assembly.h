#pragma once

#include <vector>
#include <memory>

#include "../common/typedef.h"
#include "../common/console_logger.h"
#include "../common/str_util.h"
#include "../pe/pe.h"
#include "../pe/pe_image.h"
#include "../pe/metadata_token.h"
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
            const std::vector<std::unique_ptr<MethodDef>>& get_methods();

            void read_module();
            void read_method(u32 rid);

        private:
            void read_method_body(MethodDef* method);
            void read_method_body_instructions(MethodDef* method, u8* method_body_ptr);
            std::wstring read_us_string(u32 index);
            std::wstring read_string(u32 index);
            u32 read_string_index(u8** index_ptr);
            pe::MetadataToken read_metadata_token(u8** ptr);

            u8* get_table_row_ptr(pe::MetadataTable table_type, u32 rid);
            u8* get_method_body_ptr(u32 rva);
            u32 resolve_rva(u32 rva);
            const pe::SectionTable* resolve_rva_section(u32 rva);

            std::unique_ptr<logging::ILogger> logger_;
            const pe::PEImage* image_;
            std::unique_ptr<ModuleDef> module_;
            std::vector<std::unique_ptr<MethodDef>> methods_;
        };

    }
}