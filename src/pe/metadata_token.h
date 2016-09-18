#pragma once

#include "../common/typedef.h"
#include "pe.h"

namespace stereo {
    namespace pe {

        enum class MetadataTokenType : u8
        {
            Module = 0x00,
            TypeRef = 0x01,
            TypeDef = 0x02,
            Field = 0x04,
            Method = 0x06,
            Param = 0x08,
            InterfaceImpl = 0x09,
            MemberRef = 0x0a,
            CustomAttribute = 0x0c,
            Permission = 0x0e,
            Signature = 0x11,
            Event = 0x14,
            Property = 0x17,
            ModuleRef = 0x1a,
            TypeSpec = 0x1b,
            Assembly = 0x20,
            AssemblyRef = 0x23,
            File = 0x26,
            ExportedType = 0x27,
            ManifestResource = 0x28,
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
            String = 0x70
        };

        struct MetadataToken
        {
            MetadataToken(u32 val) : value(val) {}
            MetadataToken(MetadataTokenType token_type, u32 rid) : value((static_cast<u32>(token_type) << 24) | rid) {}

            u32 value;

            u32 rid()
            {
                return value & 0x00ffffff;
            }

            MetadataTokenType type()
            {
                auto type = (value & 0xff000000) >> 24;
                return static_cast<MetadataTokenType>(type);
            }
        };

    }
}
