#pragma once
#include "sol.hpp"

namespace Blazr {
struct AnimationComponent {
	int numFrames{1}, frameRate{1}, frameOffset{0}, currentFrame{0};
	bool bVertical{false};

	static void CreateAnimationLuaBind(sol::state &lua);
};
} // namespace Blazr
