#pragma once

#include <vector>
#include <memory>

#include "../common/typedef.h"
#include "../common/console_logger.h"
#include "../common/str_util.h"
#include "../pe/pe.h"
#include "../pe/pe_image.h"
#include "../pe/metadata_token.h"
#include "../pe/coded_index.h"
#include "method_def.h"
#include "module_def.h"
#include "type_ref.h"
#include "member_ref.h"

namespace stereo {
    namespace assemblies {
        
        class Assembly;

        class AssemblyReader
        {
        public: 
            AssemblyReader(const char* file_path, Assembly* assembly);

            u32 get_entry_point();
            u32 get_num_entries(pe::MetadataTable table);
            std::unique_ptr<ModuleDef> read_module_def();
            std::unique_ptr <MethodDef> read_method_def(u32 rid);
            std::unique_ptr<MemberRef> read_member_ref(u32 rid);
            std::unique_ptr<TypeRef> read_type_ref(u32 rid);

        private:
            Assembly* assembly_;
            void read_method_body(MethodDef* method);
            void read_method_body_instructions(MethodDef* method, u8* method_body_ptr);
            std::wstring read_us_string(u32 index);
            std::wstring read_string(u8** index_ptr);
            u32 read_string_index(u8** index_ptr);
            u32 read_blob_index(u8** index_ptr);
            pe::MetadataToken read_metadata_token(u8** ptr);
            pe::MetadataToken read_metadata_token(u8** ptr, pe::CodedIndexType index_type);

            u8* get_table_row_ptr(pe::MetadataTable table_type, u32 rid);
            u8* get_method_body_ptr(u32 rva);
            u32 resolve_rva(u32 rva);
            const pe::SectionTable* resolve_rva_section(u32 rva);

            std::unique_ptr<logging::ILogger> logger_;
            std::unique_ptr<pe::PEImage> image_;
        };

    }
}