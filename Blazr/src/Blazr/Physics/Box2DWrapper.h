#pragma once

#include "box2d/box2d.h"
#include <memory>

namespace Blazr {

struct BodyDestroyer {
	void operator()(b2Body *body) const { body->GetWorld()->DestroyBody(body); }
};
static std::shared_ptr<b2Body> CreateSharedBody(b2Body *body) {
	return std::shared_ptr<b2Body>(body, BodyDestroyer{});
}
} // namespace Blazr
