#pragma once

#include <string>

#include "../common/typedef.h"

namespace stereo {
    namespace assemblies {

        struct IOperand {};

        struct InlineString : public IOperand
        {
            InlineString(const std::wstring& value) : value(value) {}
            std::wstring value;
        };

        union InlineValue
        {
            u8 byte;
            i8 sbyte;
            i32 int32;
            i64 int64;
        };

        enum class InlineValueType
        {
            Byte,
            SByte,
            Int32,
            Int64
        };

        struct InlineValueOperand : public IOperand
        {
            inline InlineValueOperand(u8 val) { value.byte = val; type = InlineValueType::Byte; }
            inline InlineValueOperand(i8 val) { value.sbyte = val; type = InlineValueType::SByte; }
            inline InlineValueOperand(i32 val) { value.int32 = val; type = InlineValueType::Int32; }
            inline InlineValueOperand(i64 val) { value.int64 = val; type = InlineValueType::Int64; }

            InlineValue value;
            InlineValueType type;
        };
    }
}