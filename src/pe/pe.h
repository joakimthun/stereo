#pragma once

#include <stdint.h>
#include <vector>
#include <memory>

#include "../common/typedef.h"

namespace stereo {
    namespace pe {

        enum class MetadataTable : u8
        {
            Module = 0x00,
            TypeRef = 0x01,
            TypeDef = 0x02,
            FieldPtr = 0x03,
            Field = 0x04,
            MethodPtr = 0x05,
            Method = 0x06,
            ParamPtr = 0x07,
            Param = 0x08,
            InterfaceImpl = 0x09,
            MemberRef = 0x0a,
            Constant = 0x0b,
            CustomAttribute = 0x0c,
            FieldMarshal = 0x0d,
            DeclSecurity = 0x0e,
            ClassLayout = 0x0f,
            FieldLayout = 0x10,
            StandAloneSig = 0x11,
            EventMap = 0x12,
            EventPtr = 0x13,
            Event = 0x14,
            PropertyMap = 0x15,
            PropertyPtr = 0x16,
            Property = 0x17,
            MethodSemantics = 0x18,
            MethodImpl = 0x19,
            ModuleRef = 0x1a,
            TypeSpec = 0x1b,
            ImplMap = 0x1c,
            FieldRVA = 0x1d,
            EncLog = 0x1e,
            EncMap = 0x1f,
            Assembly = 0x20,
            AssemblyProcessor = 0x21,
            AssemblyOS = 0x22,
            AssemblyRef = 0x23,
            AssemblyRefProcessor = 0x24,
            AssemblyRefOS = 0x25,
            File = 0x26,
            ExportedType = 0x27,
            ManifestResource = 0x28,
            NestedClass = 0x29,
            GenericParam = 0x2a,
            MethodSpec = 0x2b,
            GenericParamConstraint = 0x2c,

            Document = 0x30,
            MethodDebugInformation = 0x31,
            LocalScope = 0x32,
            LocalVariable = 0x33,
            LocalConstant = 0x34,
            ImportScope = 0x35,
            StateMachineMethod = 0x36,
            CustomDebugInformation = 0x37,
        };

        enum class CodedIndexType : u8
        {
            TypeDefOrRef,
            HasConstant,
            HasCustomAttribute,
            HasFieldMarshal,
            HasDeclSecurity,
            MemberRefParent,
            HasSemantics,
            MethodDefOrRef,
            MemberForwarded,
            Implementation,
            CustomAttributeType,
            ResolutionScope,
            TypeOrMethodDef,
            HasCustomDebugInformation,
        };

#pragma pack(push, 1)

        struct MSDOSHeader
        {
            u8 msdos_sig[2];
            u8 not_used[126];
            //u16 nlast_page;
            //u16 npages;
            //u8 msdos_header[54];
            //u32 pe_offset;
            //u8 msdos_header2[64];
        };

        struct PEFileHeader
        {
            u16  machine;
            u16  num_sections;
            u32  time_stamp;
            u32  sym_tbl_ptr;
            u32  num_sym;
            u16  opt_header_size;
            u16  characteristics;
        };

        struct PEHeader
        {
            u16 magic;
            u8 l_major;
            u8 l_minor;
            u32 code_size;
            u32 initialized_data_size;
            u32 uninitialized_data_size;
            u32 entry_point_rva;
            u32 code_base_rva;
            u32 data_base_rva;
        };

        struct PEHeaderNT
        {
            u32 image_base;
            u32 section_align;
            u32 file_alignment;		// Should be 0x200
            u16 os_major;			// Should be 5
            u16 os_minor;			// Should be 0
            u16 user_major;
            u16 user_minor;
            u16 subsys_major;
            u16 subsys_minor;
            u32 reserved;
            u32 image_size;
            u32 header_size;
            u32 file_checksum;
            u16 subsys;
            u16 dll_flags;
            u32 stack_reserve_size;
            u32 stack_commit_size;
            u32 heap_reserve_size;
            u32 heap_commit_size;
            u32 loader_flags;
            u32 num_data_dirs;
        };

        struct PEDirEntry
        {
            u32 rva;
            u32 size;
        };

        struct PEDatadir
        {
            PEDirEntry export_table;
            PEDirEntry import_table;
            PEDirEntry resource_table;
            PEDirEntry exception_table;
            PEDirEntry certificate_table;
            PEDirEntry base_reloc_table;
            PEDirEntry debug;
            PEDirEntry copyright;
            PEDirEntry global_ptr;
            PEDirEntry tls_table;
            PEDirEntry load_config_table;
            PEDirEntry bound_import;
            PEDirEntry iat;
            PEDirEntry delay_import_desc;
            PEDirEntry cli_header;
            PEDirEntry reserved;
        };

        struct SectionTable
        {
            char name[8];
            u32 virtual_size;
            u32 virtual_address;
            u32 raw_data_size;
            u32 raw_data_ptr;
            u32 reloc_ptr;
            u32 linenum_ptr;
            u16 num_reloc;
            u16 num_lines;
            u32 characteristics;
        };

        struct PEHeaders
        {
            u8 pe_sig[4];
            PEFileHeader pe_file_header;
            PEHeader    pe_header;
            PEHeaderNT  nt_header;
            PEDatadir   datadir;
        };

        struct CLIHeader
        {
            u32	cb;
            u16 runtime_major;
            u16 runtime_minor;
            PEDirEntry metadata;
            u32        flags;
            u32        entry_point_token;
            PEDirEntry resources;
            PEDirEntry strong_name_sig;
            PEDirEntry code_manager_table;
            PEDirEntry vtable_fixups;
            PEDirEntry export_address_table_jumps;
            PEDirEntry managed_native_header;
        };

        struct StreamHeader
        {
            const u8* data;
            u32  size;
        };

        struct TableInfo
        {
            const u8* base;
            u32       rows;
            u32       row_size;
        };

        struct CLIStreamInfo
        {
            u32 res1;
            u8 major_version;
            u8 minor_version;
            u8 heap_sizes;
            u8 res2;
            u64 valid;
            u64 sorted;
        };

#pragma pack(pop)

    }
}