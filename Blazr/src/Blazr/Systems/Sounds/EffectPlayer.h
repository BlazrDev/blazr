#pragma once

#include "Effect.h"
#include "Blazr/Core/Core.h"

namespace Blazr {
    class EffectPlayer {
        private:
            static Ref<EffectPlayer> instance;

        public:
            static Ref<EffectPlayer> &GetInstance() {
                if(instance == nullptr) {
                    instance = std::make_shared<EffectPlayer>();
                }
                return instance;
            }

            EffectPlayer() = default;
            ~EffectPlayer() = default;

    };
}