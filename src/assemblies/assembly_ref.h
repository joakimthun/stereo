#pragma once

#include <string>

#include "../common/typedef.h"
#include "blob_entry.h"
#include "resolution_scope.h"

namespace stereo {
    namespace assemblies {

        enum class AssemblyFlags : u32
        {
            PublicKey = 0x0001,
            SideBySideCompatible = 0x0000,
            Retargetable = 0x0100,
            WindowsRuntime = 0x0200,
            DisableJITCompileOptimizer = 0x4000,
            EnableJITCompileTracking = 0x8000
        };

        struct AssemblyRef : public IResolutionScope
        {
            u16 major_version;
            u16 minor_version;
            u16 build_number;
            u16 revision_number;
            AssemblyFlags flags;
            std::unique_ptr<BlobEntry> public_key;
            std::unique_ptr<BlobEntry> token;
            std::wstring name;
            std::wstring culture;
            std::unique_ptr<BlobEntry> hash_value;
        };

    }
}
