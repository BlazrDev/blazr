#pragma once

extern Blazr::Application* Blazr::CreateApplication();

int main(int argc, char** argv) {
	auto app = Blazr::CreateApplication();
	app->Run();
	delete app;
}