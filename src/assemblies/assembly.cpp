#include "assembly.h"

namespace stereo {
    namespace assemblies {

        Assembly::Assembly(const char* file_path)
        {
            reader_ = std::make_unique<AssemblyReader>(file_path);
        }

        u32 Assembly::get_entry_point()
        {
            return reader_->get_entry_point();
        }

        const ModuleDef* Assembly::get_module_def()
        {
            return reader_->read_module_def();
        }

        const MethodDef* Assembly::get_method_def(u32 rid)
        {
            return reader_->read_method_def(rid);
        }

        const TypeRef* Assembly::get_type_ref(u32 rid)
        {
            return reader_->read_type_ref(rid);
        }

    }
}