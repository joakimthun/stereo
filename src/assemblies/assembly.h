#pragma once

#include "../pe/pe_image.h"

namespace stereo {
    namespace assemblies {

        class Assembly 
        {
        public: 
            Assembly(const pe::PEImage* image);

            u32 get_entry_point();

        private:
            const pe::PEImage* image_;
        };

    }
}