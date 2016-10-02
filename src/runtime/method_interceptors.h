#pragma once

#include <string>

namespace stereo {
    namespace runtime {

        class Stack;

        bool try_call_interceptor(const std::wstring& fullname, Stack& stack);

    }
}