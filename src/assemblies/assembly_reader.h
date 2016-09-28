#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "../common/typedef.h"
#include "../common/console_logger.h"
#include "../common/str_util.h"
#include "../pe/pe.h"
#include "../pe/pe_image.h"
#include "../pe/metadata_token.h"
#include "../pe/coded_index.h"
#include "opcodes.h"
#include "operand.h"
#include "blob_entry.h"
#include "method_def.h"
#include "module_def.h"
#include "type_ref.h"
#include "member_ref.h"
#include "assembly_ref.h"

namespace stereo {
    namespace assemblies {
        
        class Assembly;

        class AssemblyReader
        {
        public: 
            AssemblyReader(const char* file_path);

            u32 get_entry_point();
            const ModuleDef* read_module_def();
            const MethodDef* read_method_def(u32 rid);
            const MemberRef* read_member_ref(u32 rid);
            const TypeRef* read_type_ref(u32 rid);
            const AssemblyRef* read_assembly_ref(u32 rid);

        private:
            void read_method_body(MethodDef* method);
            void read_method_body_instructions(MethodDef* method, u8* method_body_ptr);
            const Opcode& read_opcode(u8** method_body_ptr);
            const InlineString* read_us_string(u32 index);
            std::wstring read_string(u8** index_ptr);
            u32 read_string_index(u8** index_ptr);
            u32 read_blob_index(u8** index_ptr);
            std::unique_ptr<BlobEntry> read_blob(u8** index_ptr);
            pe::MetadataToken read_metadata_token(u8** ptr);
            pe::MetadataToken read_metadata_token(u8** ptr, pe::CodedIndexType index_type);

            u8* get_table_row_ptr(pe::MetadataTable table_type, u32 rid);
            u8* get_method_body_ptr(u32 rva);
            u32 read_us_or_blob_length(const u8** blob_ptr);
            u32 resolve_rva(u32 rva);
            const pe::SectionTable* resolve_rva_section(u32 rva);
            u32 get_num_entries(pe::MetadataTable table);
            u32 get_index_from_rid(u32 rid);

            template<typename T>
            inline bool already_read(std::vector<std::unique_ptr<T>>& vec, u32 rid)
            {
                return vec.size() != 0 && vec[get_index_from_rid(rid)] != nullptr;
            }

            template<typename T>
            inline void resize_if_needed(std::vector<std::unique_ptr<T>>& vec, pe::MetadataTable table)
            {
                if (vec.size() != 0)
                    return;

                vec.resize(get_num_entries(table));
            }

            std::unique_ptr<logging::ILogger> logger_;
            std::unique_ptr<pe::PEImage> image_;
            std::unique_ptr<ModuleDef> module_;
            std::vector<std::unique_ptr<AssemblyRef>> assembly_refs_;
            std::vector<std::unique_ptr<MemberRef>> member_refs_;
            std::vector<std::unique_ptr<MethodDef>> method_defs_;
            std::vector<std::unique_ptr<TypeRef>> type_refs_;
            std::vector<std::unique_ptr<InlineString>> us_strings_;
        };

    }
}