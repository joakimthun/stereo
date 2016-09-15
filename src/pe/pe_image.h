#pragma once

#include <memory>
#include <vector>
#include <string>

#include "../common/typedef.h"
#include "pe.h"

namespace stereo {
    namespace pe {

        struct PEImage
        {
            PEImage(u8* data, u64 size)
                :
                raw_data(data),
                data_size(size),
                pe_headers({ 0 }),
                cli_section_count(0),
                cli_header({ 0 }),
                heap_strings({ 0 }),
                heap_us({ 0 }),
                heap_blob({ 0 }),
                heap_guid({ 0 }),
                heap_tables({ 0 }),
                heap_pdb({ 0 }),
                cli_stream_info({ 0 }),
                string_idx_size(0),
                guid_idx_size(0),
                blob_idx_size(0)
            {}

            ~PEImage() { delete[] raw_data; }

            const u8* raw_data;
            const u64 data_size;
            PEHeaders pe_headers;
            u32 cli_section_count;
            std::vector<std::unique_ptr<SectionTable>> cli_section_tables;
            CLIHeader cli_header;
            u16 md_version_major;
            u16 md_version_minor;
            std::wstring md_version_string;
            StreamHeader heap_strings;
            StreamHeader heap_us;
            StreamHeader heap_blob;
            StreamHeader heap_guid;
            StreamHeader heap_tables;
            StreamHeader heap_pdb;
            static const u8 MAX_NUM_METADATA_TABLES = 58;
            TableInfo tables[MAX_NUM_METADATA_TABLES];
            CLIStreamInfo cli_stream_info;
            u8 string_idx_size;
            u8 guid_idx_size;
            u8 blob_idx_size;
        };

    }
}