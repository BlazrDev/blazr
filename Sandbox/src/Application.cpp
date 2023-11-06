#include <Blazr.h>
#include <iostream>

void main() {
	Blazr::Application* app = new Blazr::Application();
	std::cout << "Hello";
	app->Run();
}