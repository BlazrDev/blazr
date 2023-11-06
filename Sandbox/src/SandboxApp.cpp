#include <Blazr.h>
#include <iostream>

class Sandbox : public Blazr::Application {
public:
	Sandbox() {

	}

	~Sandbox() {

	}
};

Blazr::Application* Blazr::CreateApplication() {
	return new Sandbox();
}