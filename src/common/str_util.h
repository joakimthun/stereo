#pragma once

#include <locale>
#include <codecvt>
#include <string>

#include "typedef.h"

namespace stereo {
    namespace strutil {

        inline std::wstring utf8str_to_utf16wstr(const std::string& bytes)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar>> converter;
            return converter.from_bytes(bytes);
        }

        inline std::wstring to_utf16wstr(const u8* bytes, u32 length)
        {
            return std::wstring(reinterpret_cast<const wchar*>(bytes), length / sizeof(wchar));
        }

    }
}