#pragma once
#include "sol.hpp"
#include <json.hpp>

namespace Blazr {
struct AnimationComponent {
	int numFrames{1}, frameRate{1}, frameOffset{0}, currentFrame{0};
	bool bVertical{false};

	static void CreateAnimationLuaBind(sol::state &lua);

	static void to_json(nlohmann::json &j,
						const AnimationComponent &animation) {
		j = nlohmann::json{{"numFrames", animation.numFrames},
						   {"frameRate", animation.frameRate},
						   {"frameOffset", animation.frameOffset},
						   {"currentFrame", animation.currentFrame},
						   {"bVertical", animation.bVertical}};
	}

	static void from_json(const nlohmann::json &j,
						  AnimationComponent &animation) {
		animation.numFrames = j.at("numFrames").get<int>();
		animation.frameRate = j.at("frameRate").get<int>();
		animation.frameOffset = j.at("frameOffset").get<int>();
		animation.currentFrame = j.at("currentFrame").get<int>();
		animation.bVertical = j.at("bVertical").get<bool>();
	}
};
} // namespace Blazr
