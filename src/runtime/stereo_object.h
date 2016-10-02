#pragma once

#include <string>
#include <memory>

#include "../common/typedef.h"
#include "../assemblies/element_type.h"

namespace stereo {
    namespace runtime {

        union StereoValue
        {
            i32 i32;
            i64 i64;
            const std::wstring* str;
        };

        class StereoObject
        {
        public:
            StereoObject();
            StereoObject(i32 val);
            StereoObject(i64 val);
            StereoObject(const std::wstring* val);

            const std::wstring& get_str_value();
        private:
            StereoValue value;
            assemblies::ElementType type;
        };
    }
}