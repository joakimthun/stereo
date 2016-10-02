#pragma once

namespace stereo {

    template<class TValue>
    struct Maybe
    {
        Maybe() : has_value(false) {}
        Maybe(const TValue value) : has_value(true), value(value) {}

        bool has_value;
        TValue value;
    };

}