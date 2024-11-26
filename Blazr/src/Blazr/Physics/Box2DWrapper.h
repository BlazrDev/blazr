#pragma once

#include "box2d/box2d.h"
#include <memory>

namespace Blazr {
using PhysicsWorld = b2WorldId;

struct BodyDestroyer {
	void operator()(b2BodyId *body) const {
		b2DestroyBody(*body);
		*body = b2_nullBodyId;
	}
};

// static std::shared_ptr<b2BodyId> CreateSharedBody(b2BodyId body) {
// 	return std::shared_ptr<b2BodyId>(&body, BodyDestroyer{});
// }
} // namespace Blazr
