#include "stereo_object.h"

namespace stereo {
    namespace runtime {

        StereoObject::StereoObject()
        {
            type = assemblies::ElementType::None;
        }

        StereoObject::StereoObject(i32 val)
        {
            value.i32 = val;
            type = assemblies::ElementType::I4;
        }

        StereoObject::StereoObject(i64 val)
        {
            value.i64 = val;
            type = assemblies::ElementType::I8;
        }

        StereoObject::StereoObject(const std::wstring* val)
        {
            value.str = val;
            type = assemblies::ElementType::String;
        }

        const std::wstring& StereoObject::get_str_value()
        {
            return *value.str;
        }
        
    }
}
