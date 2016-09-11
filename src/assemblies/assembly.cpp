#include "assembly.h"

namespace stereo {
    namespace assemblies {

        Assembly::Assembly(const pe::PEImage* image)
            :
            image_(image)
        {
        }

        u32 Assembly::get_entry_point()
        {
            auto ept = image_->cli_header.entry_point_token;
            return ept & 0x00ffffff;
        }
    }
}