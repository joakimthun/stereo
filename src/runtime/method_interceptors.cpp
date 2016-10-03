#include "method_interceptors.h"

#include "stereo_object.h"
#include "../assemblies/element_type.h"

#include <unordered_map>
#include <functional>
#include <iostream>

namespace stereo {
    namespace runtime {

        void system_write_line(StereoObject* stack, u32& sp)
        {
            auto obj = stack[sp--];
            std::wcout << obj.get_str_value() << std::endl;
        }

        static std::unordered_map<std::wstring, std::function<void(StereoObject*, u32&)>> interceptors_ = {
            std::make_pair<std::wstring, std::function<void(StereoObject*, u32&)>>(L"System.Console.WriteLine(System.String)", &system_write_line)
        };

        bool try_call_interceptor(const std::wstring& fullname, StereoObject* stack, u32& sp)
        {
            auto it = interceptors_.find(fullname);
            if (it != interceptors_.end())
            {
                it->second(stack, sp);
                return true;
            }

            return false;
        }

    }
}