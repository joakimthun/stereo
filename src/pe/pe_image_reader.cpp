#include "pe_image_reader.h"

#include <string.h>

#include "../common/file_util.h"
#include "../common/ptr_util.h"

namespace stereo {
    namespace pe {

        PEImageReader::PEImageReader()
            :
            logger_(std::make_unique<common::ConsoleLogger>()),
            current_offset_(0),
            text_section_(nullptr)
        {
        }

        std::unique_ptr<PEImage> PEImageReader::load_image(const char* file_path)
        {
            auto reader = std::make_unique<PEImageReader>();

            if (!reader->load_file(file_path))
                return nullptr;

            // All read methods must be executed in this order since the current read offset is kept as a field on the PEImageReader instance

            if (!reader->read_dos_header())
                return nullptr;

            if (!reader->read_pe_headers())
                return nullptr;

            if (!reader->read_section_tables())
                return nullptr;

            reader->set_text_section();

            if (!reader->read_cli_header())
                return nullptr;

            auto read_metadata_result = reader->read_metadata();
            if (!read_metadata_result.success)
                return nullptr;

            if (!reader->read_streams(read_metadata_result))
                return nullptr;

            if (!reader->read_tables())
                return nullptr;

            return reader->release_image();
        }

        bool PEImageReader::load_file(const char* file_path)
        {
            auto file = common::load_entire_file(file_path);

            if (file == nullptr)
                return false;

            image_ = std::make_unique<PEImage>(file->content, file->size);

            // Release the file to prevent the file's destructor to delete the file content since the image now ownes it
            file.release();

            return true;
        }

        std::unique_ptr<PEImage> PEImageReader::release_image()
        {
            return std::move(image_);
        }

        bool PEImageReader::read_dos_header()
        {
            auto num_bytes_to_read = static_cast<u32>(sizeof(MSDOSHeader));

            auto dos_header = MSDOSHeader{ 0 };
            read_from_current_offset(&dos_header, num_bytes_to_read);

            current_offset_ += num_bytes_to_read;

            // Validate DOS header
            if (dos_header.msdos_sig[0] != 0x4d || dos_header.msdos_sig[1] != 0x5a)
            {
                logger_->LogError("Invalid DOS header");
                return false;
            }

            return true;
        }

        bool PEImageReader::read_pe_headers()
        {
            auto num_bytes_to_read = static_cast<u32>(sizeof(PEHeaders));

            read_from_current_offset(&image_->pe_headers, num_bytes_to_read);

            current_offset_ += num_bytes_to_read;

            // Validate PE/COFF header
            if (image_->pe_headers.pe_sig[0] != 0x50 || image_->pe_headers.pe_sig[1] != 0x45 || image_->pe_headers.pe_sig[2] != 0x00 || image_->pe_headers.pe_sig[3] != 0x00)
            {
                logger_->LogError("Invalid PE Signature");
                return false;
            }

            if (image_->pe_headers.pe_header.magic == 0x10B)
            {
                if (image_->pe_headers.pe_file_header.opt_header_size != (sizeof(PEHeaders) - sizeof(PEFileHeader) - 4))
                {
                    logger_->LogError("Invalid header size");
                    return false;
                }
            }
            else
            {
                logger_->LogError("PE32+ not yet supported");
                return false;
            }

            return true;
        }

        bool PEImageReader::read_section_tables()
        {
            image_->cli_section_count = image_->pe_headers.pe_file_header.num_sections;
            auto num_bytes_to_read = static_cast<u32>(sizeof(SectionTable));

            for (u32 i = 0; i < image_->cli_section_count; i++)
            {
                auto t = std::make_unique<SectionTable>();
                read_from_current_offset(t.get(), num_bytes_to_read);
                image_->cli_section_tables.push_back(std::move(t));

                current_offset_ += num_bytes_to_read;
            }

            return true;
        }

        bool PEImageReader::read_cli_header()
        {
            auto cli_header_base_ptr = get_absolute_addr_offset(text_section_, image_->pe_headers.datadir.cli_header.rva);

            auto text_section_ptr = image_->raw_data + cli_header_base_ptr;
            auto num_bytes_to_read = static_cast<u32>(sizeof(CLIHeader));

            read(&image_->cli_header, num_bytes_to_read, cli_header_base_ptr);

            current_offset_ += num_bytes_to_read;

            return true;
        }

        ReadResult PEImageReader::read_metadata()
        {
            auto read_result = ReadResult{ 0 };

            auto metadata_base = get_absolute_addr_offset(text_section_, image_->cli_header.metadata.rva);
            auto base_metadata_ptr = image_->raw_data + metadata_base;
            auto metadata_ptr = const_cast<u8*>(base_metadata_ptr);

            auto signature = common::read32(metadata_ptr);

            if (signature == 0x424A5342) {

                // Skip over signature
                metadata_ptr += 4;

                image_->md_version_major = common::read16(metadata_ptr);
                metadata_ptr += 2;

                image_->md_version_minor = common::read16(metadata_ptr);

                // Skip the bytes we just read and the 4 reserved bytes
                metadata_ptr += 6;

                auto version_string_len = common::read32(metadata_ptr);
                metadata_ptr += 4;

                char buffer[255];
                strncpy(buffer, (char*)metadata_ptr, version_string_len);
                image_->md_version_string = std::string(buffer, version_string_len);

                metadata_ptr += version_string_len;

                auto pad = metadata_ptr - base_metadata_ptr;
                if (pad % 4)
                    metadata_ptr += 4 - (pad % 4);
            }
            else
            {
                logger_->LogError("Invalid metadata");
                return read_result;
            }

            // skip flags
            metadata_ptr += 2;

            read_result.current_read_position_ptr = metadata_ptr;
            read_result.success = true;

            return read_result;
        }

        bool PEImageReader::read_streams(ReadResult read_result)
        {
            auto metadata_base = get_absolute_addr_offset(text_section_, image_->cli_header.metadata.rva);
            auto base_metadata_ptr = image_->raw_data + metadata_base;
            auto stream_ptr = read_result.current_read_position_ptr;

            auto num_streams = common::read16(stream_ptr);
            stream_ptr += 2;

            for (auto i = 0; i < num_streams; i++)
            {
                if (strncmp((char*)stream_ptr + 8, "#~", 3) == 0)
                {
                    image_->heap_tables.data = base_metadata_ptr + common::read32(stream_ptr);
                    image_->heap_tables.size = common::read32(stream_ptr + 4);
                    stream_ptr += 8 + 3;
                }
                else if (strncmp((char*)stream_ptr + 8, "#Strings", 9) == 0)
                {
                    image_->heap_strings.data = base_metadata_ptr + common::read32(stream_ptr);
                    image_->heap_strings.size = common::read32(stream_ptr + 4);
                    stream_ptr += 8 + 9;
                }
                else if (strncmp((char*)stream_ptr + 8, "#US", 4) == 0)
                {
                    image_->heap_us.data = base_metadata_ptr + common::read32(stream_ptr);
                    image_->heap_us.size = common::read32(stream_ptr + 4);
                    stream_ptr += 8 + 4;
                }
                else if (strncmp((char*)stream_ptr + 8, "#Blob", 6) == 0)
                {
                    image_->heap_blob.data = base_metadata_ptr + common::read32(stream_ptr);
                    image_->heap_blob.size = common::read32(stream_ptr + 4);
                    stream_ptr += 8 + 6;
                }
                else if (strncmp((char*)stream_ptr + 8, "#GUID", 6) == 0)
                {
                    image_->heap_guid.data = base_metadata_ptr + common::read32(stream_ptr);
                    image_->heap_guid.size = common::read32(stream_ptr + 4);
                    stream_ptr += 8 + 6;
                }
                else if (strncmp((char*)stream_ptr + 8, "#Pdb", 5) == 0)
                {
                    image_->heap_pdb.data = base_metadata_ptr + common::read32(stream_ptr);
                    image_->heap_pdb.size = common::read32(stream_ptr + 4);
                    stream_ptr += 8 + 5;
                }
                else
                {
                    logger_->LogWarning("Unknown heap type");
                    stream_ptr += 8 + strlen((char*)stream_ptr + 8) + 1;
                }

                auto pad = stream_ptr - base_metadata_ptr;
                if (pad % 4)
                    stream_ptr += 4 - (pad % 4);
            }

            return true;
        }

        bool PEImageReader::read_tables()
        {
            // Zero out all tables
            for (auto i = 0; i < PEImage::MAX_NUM_METADATA_TABLES; i++)
            {
                image_->tables[i] = TableInfo{ 0 };
            }

            auto heap_tables_ptr_base = image_->heap_tables.data;
            auto heap_tables_ptr = (u8*)heap_tables_ptr_base;
            auto cli_stream_info_size = static_cast<u32>(sizeof(CLIStreamInfo));

            read(&image_->cli_stream_info, cli_stream_info_size, heap_tables_ptr);

            image_->string_idx_size = ((image_->cli_stream_info.heap_sizes & 0x01) == 1) ? 4 : 2;
            image_->guid_idx_size = ((image_->cli_stream_info.heap_sizes & 0x02) == 2) ? 4 : 2;
            image_->blob_idx_size = ((image_->cli_stream_info.heap_sizes & 0x04) == 4) ? 4 : 2;

            auto rows = (u32*)(heap_tables_ptr + cli_stream_info_size);
            auto valid = 0;
            const u64 shift = 1;

            // Read the number of rows in each table

            for (auto table = 0; table < 64; table++)
            {
                if ((image_->cli_stream_info.valid & (shift << table)) == 0)
                {
                    if (table > PEImage::MAX_NUM_METADATA_TABLES)
                        continue;

                    image_->tables[table].rows = 0;
                    continue;
                }

                if (table > PEImage::MAX_NUM_METADATA_TABLES)
                {
                    logger_->LogWarning("PEImageReader::read_tables --> if (table > PEImage::MAX_NUM_METADATA_TABLES)");
                }
                else
                {
                    image_->tables[table].rows = common::read32((u8*)rows);
                }

                rows++;
                valid++;
            }

            auto tables_base_ptr = (heap_tables_ptr + cli_stream_info_size) + (4 * valid);
            if (tables_base_ptr != (u8*)rows)
            {
                logger_->LogError("Offset mismatch when reading the number of rows in each table");
                return false;
            }

            // Set table base pointers
            auto current_table_base_ptr = tables_base_ptr;
            for (auto i = 0; i < PEImage::MAX_NUM_METADATA_TABLES; i++)
            {
                auto table = &image_->tables[i];
                if (table->rows == 0)
                    continue;

                table->row_size = compute_metadata_row_size(static_cast<MetadataTable>(i));
                table->base = current_table_base_ptr;
                current_table_base_ptr += table->rows * table->row_size;
            }

            return true;
        }

        void PEImageReader::set_text_section()
        {
            for (const auto& s : image_->cli_section_tables)
            {
                if (s->name[1] == 't' && s->name[2] == 'e' && s->name[3] == 'x' && s->name[4] == 't')
                    text_section_ = s.get();
            }
        }

        u32 PEImageReader::get_absolute_addr_offset(const SectionTable* section, u32 rva)
        {
            auto start_offset = section->raw_data_ptr;
            auto offset = rva - section->virtual_address;

            return start_offset + offset;
        }

        void PEImageReader::read_from_current_offset(void* dest, u32 num_bytes)
        {
            const auto src = image_->raw_data + current_offset_;
            std::memcpy(dest, src, num_bytes);
        }

        void PEImageReader::read(void * dest, u32 num_bytes, u32 offset)
        {
            const auto src = image_->raw_data + offset;
            std::memcpy(dest, src, num_bytes);
        }

        void PEImageReader::read(void* dest, u32 num_bytes, const void* src)
        {
            std::memcpy(dest, src, num_bytes);
        }

        u32 PEImageReader::get_table_length(MetadataTable table_type)
        {
            return image_->tables[static_cast<int>(table_type)].rows;
        }

        u8 PEImageReader::get_table_index_size(MetadataTable table_type)
        {
            return get_table_length(table_type) < 65536 ? 2 : 4;
        }

        u8 PEImageReader::get_coded_index_size(CodedIndexType type)
        {
            // TODO: Need to check if it fits into 2 bytes, needs 4 bytes in bigger images
            return 2;
        }

        u32 PEImageReader::compute_metadata_row_size(MetadataTable table)
        {
            u8 string_idx_size = image_->string_idx_size;
            u8 guid_idx_size = image_->guid_idx_size;
            u8 blob_idx_size = image_->blob_idx_size;

            switch (table)
            {
            case MetadataTable::Module: {
                return 2					// Generation
                    + string_idx_size		// Name
                    + (guid_idx_size * 3);	// Mvid, EncId, EncBaseId
            }
            case MetadataTable::TypeRef: {
                return get_coded_index_size(CodedIndexType::ResolutionScope)	// ResolutionScope
                    + (string_idx_size * 2);									// Name, Namespace
            }
            case MetadataTable::TypeDef: {
                return 4													// Flags
                    + (string_idx_size * 2)									// Name, Namespace
                    + get_coded_index_size(CodedIndexType::TypeDefOrRef)	// BaseType
                    + get_table_index_size(MetadataTable::Field)			// FieldList
                    + get_table_index_size(MetadataTable::Method);			// MethodList
            }
            case MetadataTable::FieldPtr: {
                return get_table_index_size(MetadataTable::Field);	// Field
            }
            case MetadataTable::Field: {
                return 2				// Flags
                    + string_idx_size	// Name
                    + blob_idx_size;	// Signature
            }
            case MetadataTable::MethodPtr: {
                return get_table_index_size(MetadataTable::Method);		// Method
            }
            case MetadataTable::Method: {
                return 8											// Rva 4, ImplFlags 2, Flags 2
                    + string_idx_size								// Name
                    + blob_idx_size									// Signature
                    + get_table_index_size(MetadataTable::Param);	// ParamList
            }
            case MetadataTable::ParamPtr: {
                return get_table_index_size(MetadataTable::Param);	// Param
            }
            case MetadataTable::Param: {
                return 4					// Flags 2, Sequence 2
                    + string_idx_size;		// Name
            }
            case MetadataTable::InterfaceImpl: {
                return get_table_index_size(MetadataTable::TypeDef)			// Class
                    + get_coded_index_size(CodedIndexType::TypeDefOrRef);	// Interface
            }
            case MetadataTable::MemberRef: {
                return get_coded_index_size(CodedIndexType::MemberRefParent)	// Class
                    + string_idx_size											// Name
                    + blob_idx_size;											// Signature
            }
            case MetadataTable::Constant: {
                return 2												// Type
                    + get_coded_index_size(CodedIndexType::HasConstant)	// Parent
                    + blob_idx_size;									// Value
            }
            case MetadataTable::CustomAttribute: {
                return get_coded_index_size(CodedIndexType::HasCustomAttribute)	// Parent
                    + get_coded_index_size(CodedIndexType::CustomAttributeType)	// Type
                    + blob_idx_size;											// Value
            }
            case MetadataTable::Assembly: {
                return 16						// HashAlgId 4, Version 4 * 2, Flags 4
                    + blob_idx_size				// PublicKey
                    + (string_idx_size * 2);	// Name, Culture
            }
            case MetadataTable::AssemblyProcessor: {
                return 4;	// Processor
            }
            case MetadataTable::AssemblyOS: {
                return 12;	// Platform 4, Version 2 * 4
            }
            case MetadataTable::AssemblyRef: {
                return 12						// Version 2 * 4 + Flags 4
                    + (blob_idx_size * 2)		// PublicKeyOrToken, HashValue
                    + (string_idx_size * 2);	// Name, Culture
            }
                                             // TODO: Implement all cases....
            default:
                break;
            }

            logger_->LogError("PEImageReader::compute_metadata_row_size --> Unsupported MetaDataTable");
            return 0;
        }

    }
}