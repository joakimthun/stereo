#pragma once

#include <memory>

#include "../common/console_logger.h"
#include "../common/file_util.h"
#include "pe_image.h"

namespace stereo {
    namespace pe {

        struct ReadResult
        {
            bool success;
            u8* current_read_position_ptr;
        };

        class PEImageReader
        {
        public:
            PEImageReader();
            static std::unique_ptr<PEImage> load_image(const char* file_path);

        private:
            bool load_file(const char* file_path);
            std::unique_ptr<PEImage> release_image();
            bool read_dos_header();
            bool read_pe_headers();
            bool read_section_tables();
            bool read_cli_header();
            ReadResult read_metadata();
            bool read_streams(ReadResult read_result);
            bool read_tables();
            void set_text_section();
            u32 get_absolute_addr_offset(const SectionTable* section, u32 rva);
            void read_from_current_offset(void* dest, u32 num_bytes);
            void read(void* dest, u32 num_bytes, u32 offset);
            void read(void* dest, u32 num_bytes, const void* src);
            u32 get_table_length(MetadataTable table_type);
            u8 get_table_index_size(MetadataTable table_type);
            u8 get_coded_index_size(CodedIndexType type);
            u32 compute_metadata_row_size(MetadataTable table);

            std::unique_ptr<common::ILogger> logger_;
            std::unique_ptr<PEImage> image_;
            u64 current_offset_;
            const SectionTable* text_section_;
        };

    }
}
