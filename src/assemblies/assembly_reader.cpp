#include "assembly_reader.h"

#include <cstring>

#include "../exceptions/not_implemented_exception.h"
#include "../common/ptr_util.h"
#include "opcodes.h"
#include "../pe/coded_index.h"
#include "../pe/pe_image_reader.h"
#include "assembly.h"

namespace stereo {
    namespace assemblies {

        AssemblyReader::AssemblyReader(const char* file_path)
            :
            logger_(std::make_unique<logging::ConsoleLogger>()),
            image_(pe::PEImageReader::load_image(file_path))
        {
        }

        const pe::PEImage * AssemblyReader::get_image()
        {
            return image_.get();
        }

        u32 AssemblyReader::get_entry_point()
        {
            auto ept = pe::MetadataToken(image_->cli_header.entry_point_token);
            return ept.rid();
        }

        const ModuleDef* AssemblyReader::read_module_def()
        {
            if (module_ != nullptr)
                return module_.get();

            auto module = std::make_unique<ModuleDef>();

            auto table_row_ptr = get_table_row_ptr(pe::MetadataTable::Module, 1);

            // Skip the generation column since it's always zero
            table_row_ptr += 2;

            module->name = read_string(&table_row_ptr);

            // TODO: Read the mvid guid

            module_ = std::move(module);
            return module_.get();
        }

        const MethodDef* AssemblyReader::read_method_def(u32 rid)
        {
            if (already_read(method_defs_, rid))
                return method_defs_[get_index_from_rid(rid)].get();

            resize_if_needed(method_defs_, pe::MetadataTable::Method);

            auto table_row_ptr = get_table_row_ptr(pe::MetadataTable::Method, rid);
            auto method_def = std::make_unique<MethodDef>();

            // RVA (a 4-byte constant) 
            method_def->rva = ptrutil::read32(&table_row_ptr);

            // ImplFlags (a 2-byte bitmask of type MethodImplAttributes, �II.23.1.10) 
            method_def->impl_attributes = static_cast<MethodImplAttributes>(ptrutil::read16(&table_row_ptr));

            // Flags (a 2-byte bitmask of type MethodAttributes, �II.23.1.10) 
            method_def->attributes = static_cast<MethodAttributes>(ptrutil::read16(&table_row_ptr));

            // Name (an index into the String heap) 
            method_def->name = read_string(&table_row_ptr);

            // TODO: Read signature, parameters etc

            read_method_body(method_def.get());

            method_defs_[get_index_from_rid(rid)] = std::move(method_def);
            return method_defs_[get_index_from_rid(rid)].get();
        }

        const MemberRef* AssemblyReader::read_member_ref(u32 rid)
        {
            if (already_read(member_refs_, rid))
                return member_refs_[get_index_from_rid(rid)].get();

            resize_if_needed(member_refs_, pe::MetadataTable::MemberRef);

            auto table_row_ptr = get_table_row_ptr(pe::MetadataTable::MemberRef, rid);
            //auto member_ref = std::make_unique<MemberRef>();

            // Class (an index into the MethodDef, ModuleRef, TypeDef, TypeRef, or TypeSpec tables; more precisely, a MemberRefParent(�II.24.2.6) coded index)
            auto token = read_metadata_token(&table_row_ptr, pe::CodedIndexType::MemberRefParent);

            // Name (an index into the String heap) 
            auto name = read_string(&table_row_ptr);

            // Signature (an index into the Blob heap) 
            auto signature = read_blob(&table_row_ptr);

            // TODO: Implement all token type cases
            if (token.type() == pe::MetadataTokenType::TypeRef)
            {
                auto declaring_type = read_type_ref(token.rid());
                auto member_ref = read_member_ref_sig(signature.get(), name, declaring_type);

                member_refs_[get_index_from_rid(rid)] = std::move(member_ref);
                return member_refs_[get_index_from_rid(rid)].get();
            }
            else
            {
                throw exceptions::NotImplementedException(L"read_member_ref -> unsupported token type");
            }
        }

        const TypeRef* AssemblyReader::read_type_ref(u32 rid)
        {
            if (already_read(type_refs_, rid))
                return type_refs_[get_index_from_rid(rid)].get();

            resize_if_needed(type_refs_, pe::MetadataTable::TypeRef);

            auto table_row_ptr = get_table_row_ptr(pe::MetadataTable::TypeRef, rid);

            auto type_ref = std::make_unique<TypeRef>();

            // ResolutionScope (an index into a Module, ModuleRef, AssemblyRef or TypeRef table, or null; more precisely, a ResolutionScope(�II.24.2.6) coded index)
            auto res_scope = read_metadata_token(&table_row_ptr, pe::CodedIndexType::ResolutionScope);

            if (res_scope.type() == pe::MetadataTokenType::AssemblyRef)
            {
                type_ref->resolution_scope = read_assembly_ref(res_scope.rid());
            }
            else
            {
                throw exceptions::NotImplementedException(L"read_type_ref -> unsupported ResolutionScope type");
            }

            // TypeName(an index into the String heap)
            type_ref->name = read_string(&table_row_ptr);

            // TypeNamespace(an index into the String heap)
            type_ref->name_space = read_string(&table_row_ptr);

            type_refs_[get_index_from_rid(rid)] = std::move(type_ref);
            return type_refs_[get_index_from_rid(rid)].get();
        }

        const AssemblyRef* AssemblyReader::read_assembly_ref(u32 rid)
        {
            if (already_read(assembly_refs_, rid))
            {
                return assembly_refs_[get_index_from_rid(rid)].get();
            }

            resize_if_needed(assembly_refs_, pe::MetadataTable::AssemblyRef);

            auto table_row_ptr = get_table_row_ptr(pe::MetadataTable::AssemblyRef, rid);

            auto asm_ref = std::make_unique<AssemblyRef>();

            // MajorVersion, MinorVersion, BuildNumber, RevisionNumber (each being 2-byte constants)
            asm_ref->major_version = ptrutil::read16(&table_row_ptr);
            asm_ref->minor_version = ptrutil::read16(&table_row_ptr);
            asm_ref->build_number = ptrutil::read16(&table_row_ptr);
            asm_ref->revision_number = ptrutil::read16(&table_row_ptr);

            auto flags = ptrutil::read32(&table_row_ptr);

            auto key_or_token_blob = read_blob(&table_row_ptr);
            
            //PublicKeyOrToken (an index into the Blob heap, indicating the public key or token that identifies the author of this Assembly)
            if ((static_cast<u32>(asm_ref->flags) & static_cast<u32>(AssemblyFlags::PublicKey)) != 0)
            {
                asm_ref->public_key = std::move(key_or_token_blob);
            }
            else
            {
                asm_ref->token = std::move(key_or_token_blob);
            }

            // Name (an index into the String heap)
            asm_ref->name = read_string(&table_row_ptr);

            // Culture (an index into the String heap) 
            asm_ref->culture = read_string(&table_row_ptr);

            // HashValue (an index into the Blob heap)
            asm_ref->hash_value = read_blob(&table_row_ptr);

            assembly_refs_[get_index_from_rid(rid)] = std::move(asm_ref);
            return assembly_refs_[get_index_from_rid(rid)].get();
        }

        void AssemblyReader::read_method_body(MethodDef* method)
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

                read_method_body_instructions(method, method_body_ptr);
            }
            else
            {
                throw exceptions::NotImplementedException(L"AssemblyReader::read_method_body -> Not implement fat header reading");
            }
        }

        void AssemblyReader::read_method_body_instructions(MethodDef* method, u8* method_body_ptr)
        {
            auto opcode = read_opcode(&method_body_ptr);
            auto stop_addr = method_body_ptr + method->body->code_size;

            while (method_body_ptr != stop_addr)
            {
                switch (opcode.operand_type)
                {
                case OperandType::InlineNone: {
                    method->body->instructions.push_back(std::make_unique<InstructionBase>(opcode));
                    break;
                }
                case OperandType::InlineString: {
                    auto str_token = read_metadata_token(&method_body_ptr);
                    auto str = read_us_string(str_token.rid());
                    method->body->instructions.push_back(std::make_unique<PtrInstruction>(opcode, str));
                    break;
                }
                case OperandType::ShortInlineI: {
                    if (opcode.code == Code::LDC_I4_S)
                    {
                        method->body->instructions.push_back(std::make_unique<UniquePtrInstruction>(opcode, std::make_unique<InlineValueOperand>(static_cast<i8>(ptrutil::read8(&method_body_ptr)))));
                    }
                    else
                    {
                        method->body->instructions.push_back(std::make_unique<UniquePtrInstruction>(opcode, std::make_unique<InlineValueOperand>(ptrutil::read8(&method_body_ptr))));
                    }
                    break;
                }
                case OperandType::InlineI: {
                    method->body->instructions.push_back(std::make_unique<UniquePtrInstruction>(opcode, std::make_unique<InlineValueOperand>(static_cast<i32>(ptrutil::read32(&method_body_ptr)))));
                    break;
                }
                case OperandType::InlineI8: {
                    method->body->instructions.push_back(std::make_unique<UniquePtrInstruction>(opcode, std::make_unique<InlineValueOperand>(static_cast<i64>(ptrutil::read64(&method_body_ptr)))));
                    break;
                }
                case OperandType::InlineTok:
                case OperandType::InlineType:
                case OperandType::InlineMethod:
                case OperandType::InlineField: {
                    auto token = read_metadata_token(&method_body_ptr);
                    method->body->instructions.push_back(std::make_unique<PtrInstruction>(opcode, read_operand(token)));
                    break;
                }
                default:
                    throw exceptions::NotImplementedException(L"AssemblyReader::read_method_body_instructions -> Unhandled OperandType");
                }

                opcode = read_opcode(&method_body_ptr);
            }
        }

        const Opcode& AssemblyReader::read_opcode(u8** method_body_ptr)
        {
            auto code = ptrutil::read8(method_body_ptr);
            return code == 0xff ? opcodes::get_two_byte_code(ptrutil::read8(method_body_ptr)) : opcodes::get_one_byte_code(code);
        }

        const InlineString* AssemblyReader::read_us_string(u32 index)
        {
            auto size = index + 1;

            if (us_strings_.size() < size)
                us_strings_.resize(size);

            if (us_strings_[index] != nullptr)
                return us_strings_[index].get();

            if (index == 0)
                return nullptr;

            auto str_ptr = image_->heap_us.data + index;

            // II.24.2.4 #US and #Blob heaps
            auto length = read_compressed_u32(&str_ptr) & 0xfffffffe;

            us_strings_[index] = std::make_unique<InlineString>(strutil::to_utf16wstr(str_ptr, length));
            return us_strings_[index].get();
        }

        std::wstring AssemblyReader::read_string(u8** index_ptr)
        {
            auto index = read_string_index(index_ptr);

            std::wstring value;

            if (index == 0)
                return value;

            auto string_ptr = image_->heap_strings.data + index;

            auto utf8_str = std::string(reinterpret_cast<const char*>(string_ptr));
            return strutil::utf8str_to_utf16wstr(utf8_str);
        }

        u32 AssemblyReader::read_string_index(u8** index_ptr)
        {
            auto index_size = image_->string_idx_size;

            u32 str_index = 0;
            if (index_size == 2)
            {
                str_index = ptrutil::read16(index_ptr);
            }
            else
            {
                str_index = ptrutil::read32(index_ptr);
            }

            return str_index;
        }

        u32 AssemblyReader::read_blob_index(u8** index_ptr)
        {
            if (image_->blob_idx_size == 2)
                return ptrutil::read16(index_ptr);

            return ptrutil::read32(index_ptr);
        }

        std::unique_ptr<BlobEntry> AssemblyReader::read_blob(u8** index_ptr)
        {
            auto index = read_blob_index(index_ptr);
            auto blob_ptr = image_->heap_blob.data + index;
            auto length = read_compressed_u32(&blob_ptr);

            auto buffer = new u8[length];
            std::memcpy(buffer, blob_ptr, length);

            return std::make_unique<BlobEntry>(buffer, length);
        }

        pe::MetadataToken AssemblyReader::read_metadata_token(u8** ptr)
        {
            auto value = ptrutil::read32(ptr);
            return pe::MetadataToken(value);
        }

        pe::MetadataToken AssemblyReader::read_metadata_token(u8** ptr, pe::CodedIndexType index_type)
        {
            auto coded_index_info = pe::get_coded_index_info(index_type, image_->tables);
            u32 token;
            if (coded_index_info.size == 2)
                token = ptrutil::read16(ptr);
            else
                token = ptrutil::read32(ptr);

            return pe::get_metadata_token_from_coded_index(coded_index_info, token);
        }

        const IOperand* AssemblyReader::read_operand(pe::MetadataToken& token)
        {
            auto rid = token.rid();
            auto type = token.type();

            switch (type)
            {
            case pe::MetadataTokenType::MemberRef: {
                return read_member_ref(rid);
            }
            case pe::MetadataTokenType::Method: {
                return read_method_def(rid);
            }
            default:
                throw exceptions::NotImplementedException(L"AssemblyReader::read_operand -> Unsupported token type");
            }
        }

        std::unique_ptr<MemberRef> AssemblyReader::read_member_ref_sig(const BlobEntry* sig, const std::wstring& name, const TypeRef* declaring_type)
        {
            const u8 field_sig = 0x6;
            if (sig->data[0] == field_sig)
            {
                throw exceptions::NotImplementedException(L"AssemblyReader::read_member_ref_sig -> field_sig");
            }
            else 
            {
                auto method_ref = std::make_unique<MethodRef>(name, declaring_type);

                read_method_sig(sig, method_ref.get());

                return std::move(method_ref);
            }
        }

        void AssemblyReader::read_method_sig(const BlobEntry* sig, MethodRef* method)
        {
            u8* data = sig->data;
            auto calling_convention = ptrutil::read8(&data);

            const u8 has_this = 0x20;
            const u8 explicit_this = 0x40;

            if ((calling_convention & has_this) != 0) 
            {
                method->has_this = true;
                calling_convention = (u8)(calling_convention & ~has_this);
            }

            if ((calling_convention & explicit_this) != 0) 
            {
                method->explicit_this = true;
                calling_convention = (u8)(calling_convention & ~explicit_this);
            }

            method->calling_convention = static_cast<CallingConvention>(calling_convention);
            method->num_params = read_compressed_u32(const_cast<const u8**>(&data));
            method->return_type = read_type_sig(ptrutil::read8(&data));

            if (method->num_params == 0)
                return;

            for (u32 i = 0; i < method->num_params; i++)
                method->parameters.push_back(std::make_unique<ParameterDef>(read_type_sig(ptrutil::read8(&data))));
        }

        const TypeRef * AssemblyReader::read_type_sig(u8 element_type)
        {
            auto type = static_cast<ElementType>(element_type);

            switch (type)
            {
            case ElementType::Void:
            case ElementType::Boolean:
            case ElementType::Char:
            case ElementType::I1:
            case ElementType::U1:
            case ElementType::I2:
            case ElementType::U2:
            case ElementType::I4:
            case ElementType::U4:
            case ElementType::I8:
            case ElementType::U8:
            case ElementType::R4:
            case ElementType::R8:
            case ElementType::String:
                return get_primitive_type(type);
            default:
                throw exceptions::NotImplementedException(L"AssemblyReader::read_type_sig -> Unsupported element type");
            }
        }

        u8* AssemblyReader::get_table_row_ptr(pe::MetadataTable table_type, u32 rid)
        {
            auto table = image_->tables[static_cast<int>(table_type)];
            if (table.rows == 0)
                return nullptr;

            return const_cast<u8*>(table.base + (table.row_size * (rid - 1)));
        }

        u8* AssemblyReader::get_method_body_ptr(u32 rva)
        {
            return const_cast<u8*>(image_->raw_data) + resolve_rva(rva);
        }

        u32 AssemblyReader::read_compressed_u32(const u8** blob_ptr)
        {
            u32 length;
            auto ptr = *blob_ptr;

            // II.24.2.4 #US and #Blob heaps
            if ((ptr[0] & 0x80) == 0)
            {
                *blob_ptr += 1;
                length = ptr[0];
            }
            else if ((ptr[0] & 0x40) == 0)
            {
                *blob_ptr += 2;
                length = (u32)(ptr[0] & ~0x80) << 8;
                length |= ptr[1];
            }
            else
            {
                *blob_ptr += 4;
                length = (u32)(ptr[0] & ~0xc0) << 24;
                length |= (u32)ptr[1] << 16;
                length |= (u32)ptr[2] << 8;
                length |= (u32)ptr[3];
            }

            return length;
        }

        u32 AssemblyReader::resolve_rva(u32 rva)
        {
            auto section = resolve_rva_section(rva);
            return rva + section->raw_data_ptr - section->virtual_address;
        }

        const pe::SectionTable* AssemblyReader::resolve_rva_section(u32 rva)
        {
            for (auto& s : image_->cli_section_tables)
            {
                if (rva >= s->virtual_address && rva < s->virtual_address + s->raw_data_size)
                    return s.get();
            }

            return nullptr;
        }

        u32 AssemblyReader::get_num_entries(pe::MetadataTable table)
        {
            auto table_info = image_->tables[static_cast<int>(table)];
            return table_info.rows;
        }

        u32 AssemblyReader::get_index_from_rid(u32 rid)
        {
            return rid - 1;
        }
    }
}
