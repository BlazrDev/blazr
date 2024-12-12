#pragma once
#include "Application.h"

#include "Blazr/Events/Event.h"
#include "Blazr/Events/KeyEvent.h"
#include "Log.h"

extern Blazr::Application *Blazr::CreateApplication();

int main(int argc, char **argv) {
  Blazr::Log::init();
  BLZR_CORE_WARN("Starting Blazr!");
  int a = 5;
  BLZR_CLIENT_INFO("Hello! Var={0}", a);

  auto app = Blazr::CreateApplication();
  app->Run();
  //delete app;
}
