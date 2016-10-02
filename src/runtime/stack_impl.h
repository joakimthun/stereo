#pragma once

#include "../common/typedef.h"
#include "../common/maybe.h"

namespace stereo {
    namespace runtime {
    
        template<class T, u32 Size>
        class StackImpl
        {
        public:
            StackImpl() : sp_(0), has_values_(false) {}

            inline void push(const T& value)
            {
                has_values_ = true;

                if (sp_ == 0)
                {
                    stack_[sp_] = value;
                    return;
                }

                stack_[++sp_] = value;
            }

            inline Maybe<T> pop()
            {
                if (sp_ == 0 && !has_values_)
                {
                    return Maybe<T>();
                }

                if (sp_ == 0 && has_values_)
                {
                    has_values_ = false;
                    return Maybe<T>(stack_[sp_]);
                }

                if(sp_ == 1)
                    has_values_ = false;

                return Maybe<T>(stack_[sp_--]);
            }

            inline T* at(u32 index)
            {
                if (!has_values_)
                {
                    return nullptr;
                }

                return &stack_[safe_index(index)];
            }

            inline T* top()
            {
                if (!has_values_)
                {
                    return nullptr;
                }

                return &stack_[sp_];
            }

            inline u32 sp()
            {
                return sp_;
            }

        private:
            inline u32 safe_index(u32 index)
            {
                if (index <= 0) return 0;
                if (index >= sp_) return sp_;

                return index;
            }

            bool has_values_;
            u32 sp_;
            T stack_[Size];
        };

    }
}