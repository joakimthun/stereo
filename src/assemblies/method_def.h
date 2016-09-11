#pragma once

#include <string>

#include "../common/typedef.h"

namespace stereo {
    namespace assemblies {

        enum class MethodImplAttributes : u16
        {
            CodeTypeMask = 0x0003,
            IL = 0x0000,	// Method impl is CIL
            Native = 0x0001,	// Method impl is native
            OPTIL = 0x0002,	// Reserved: shall be zero in conforming implementations
            Runtime = 0x0003,	// Method impl is provided by the runtime

            ManagedMask = 0x0004,	// Flags specifying whether the code is managed or unmanaged
            Unmanaged = 0x0004,	// Method impl is unmanaged, otherwise managed
            Managed = 0x0000,	// Method impl is managed

            // Implementation info and interop
            ForwardRef = 0x0010,	// Indicates method is defined; used primarily in merge scenarios
            PreserveSig = 0x0080,	// Reserved: conforming implementations may ignore
            InternalCall = 0x1000,	// Reserved: shall be zero in conforming implementations
            Synchronized = 0x0020,	// Method is single threaded through the body
            NoOptimization = 0x0040,	// Method is not optimized by the JIT.
            NoInlining = 0x0008,	// Method may not be inlined
        };

        enum class MethodAttributes : u16
        {
            MemberAccessMask = 0x0007,
            CompilerControlled = 0x0000,	// Member not referenceable
            Private = 0x0001,	// Accessible only by the parent type
            FamANDAssem = 0x0002,	// Accessible by sub-types only in this Assembly
            Assembly = 0x0003,	// Accessibly by anyone in the Assembly
            Family = 0x0004,	// Accessible only by type and sub-types
            FamORAssem = 0x0005,	// Accessibly by sub-types anywhere, plus anyone in assembly
            Public = 0x0006,	// Accessibly by anyone who has visibility to this scope

            Static = 0x0010,	// Defined on type, else per instance
            Final = 0x0020,	// Method may not be overridden
            Virtual = 0x0040,	// Method is virtual
            HideBySig = 0x0080,	// Method hides by name+sig, else just by name

            VtableLayoutMask = 0x0100,	// Use this mask to retrieve vtable attributes
            ReuseSlot = 0x0000,	// Method reuses existing slot in vtable
            NewSlot = 0x0100,	// Method always gets a new slot in the vtable

            CheckAccessOnOverride = 0x0200,   // Method can only be overriden if also accessible
            Abstract = 0x0400,	// Method does not provide an implementation
            SpecialName = 0x0800,	// Method is special

                                    // Interop Attributes
            PInvokeImpl = 0x2000,	// Implementation is forwarded through PInvoke
            UnmanagedExport = 0x0008,	// Reserved: shall be zero for conforming implementations

            // Additional flags
            RTSpecialName = 0x1000,	// CLI provides 'special' behavior, depending upon the name of the method
            HasSecurity = 0x4000,	// Method has security associate with it
            RequireSecObject = 0x8000	 // Method calls another method containing security code
        };

        struct MethodDef
        {
            u32 rva;
            MethodImplAttributes impl_attributes;
            MethodAttributes attributes;
            std::wstring name;
        };

    }
}
