#include "method_interceptors.h"

#include "stack.h"
#include "../assemblies/element_type.h"

#include <unordered_map>
#include <functional>
#include <iostream>

namespace stereo {
    namespace runtime {

        void system_write_line(Stack& stack)
        {
            auto obj = stack.pop();
            if (obj.has_value)
            {
                std::wcout << obj.value.get_str_value() << std::endl;
            }
            else
            {
                throw "system_write_line -> No arg on stack";
            }
        }

        static std::unordered_map<std::wstring, std::function<void(Stack&)>> interceptors_ = {
            std::make_pair<std::wstring, std::function<void(Stack&)>>(L"System.Console.WriteLine(System.String)", &system_write_line)
        };

        bool try_call_interceptor(const std::wstring& fullname, Stack& stack)
        {
            auto it = interceptors_.find(fullname);
            if (it != interceptors_.end())
            {
                it->second(stack);
                return true;
            }

            return false;
        }

    }
}