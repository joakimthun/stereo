#pragma once

#include <vector>
#include <memory>

#include "../common/typedef.h"
#include "assembly_reader.h"
#include "module_def.h"
#include "method_def.h"
#include "type_ref.h"

namespace stereo {
    namespace assemblies {

        class Assembly
        {
        public:
            Assembly(const char* file_path);

            u32 get_entry_point();
            const ModuleDef* get_module_def();
            const MethodDef* get_method_def(u32 rid);
            const TypeRef* get_type_ref(u32 rid);

        private:
            std::unique_ptr<AssemblyReader> reader_;
            std::unique_ptr<ModuleDef> module_;
            std::vector<std::unique_ptr<MethodDef>> method_defs_;
            std::vector<std::unique_ptr<TypeRef>> type_refs_;
        };

    }
}