#pragma once
#include "box2d/box2d.h"
#include <memory>

namespace Blazr {

struct BodyDestroyer {
	std::shared_ptr<b2World> world;

	void operator()(b2Body *body) const {
		if (world) {
			world->DestroyBody(body);
		}
	}
};

static std::shared_ptr<b2Body>
CreateSharedBody(b2Body *body, std::shared_ptr<b2World> world) {
	return std::shared_ptr<b2Body>(body, BodyDestroyer{world});
}
} // namespace Blazr
