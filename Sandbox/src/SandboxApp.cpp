#include "Blazr.h"
#include <iostream>

namespace Blazr {
class Sandbox : public Application {
  public:
	Sandbox() {}

	~Sandbox() {}
};

Application *CreateApplication() { return new Sandbox(); }
} // namespace Blazr
