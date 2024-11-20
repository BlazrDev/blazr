#pragma once


#include <string>

namespace Blazr {
    struct SoundProperties {
        std::string name{""}, description{""}, filename{""};
        double duration{0.0};
    };
}