#include "assembly.h"

namespace stereo {
    namespace assemblies {

        Assembly::Assembly(const char* file_path)
        {
            reader_ = std::make_unique<AssemblyReader>(file_path, this);
        }

        u32 Assembly::get_entry_point()
        {
            return reader_->get_entry_point();
        }

        const ModuleDef * Assembly::get_module_def()
        {
            if (module_ == nullptr)
                module_ = reader_->read_module_def();

            return module_.get();
        }

        const MethodDef * Assembly::get_method_def(u32 rid)
        {
            if (method_defs_.size() == 0)
                method_defs_.resize(reader_->get_num_entries(pe::MetadataTable::Method));

            if (method_defs_[rid] == nullptr)
                method_defs_[rid] = reader_->read_method_def(rid);

            return method_defs_[rid].get();
        }

        const TypeRef * Assembly::get_type_ref(u32 rid)
        {
            return nullptr;
        }

    }
}