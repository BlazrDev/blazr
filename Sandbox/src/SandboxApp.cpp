#include "Blazr.h"
#include "Blazr/Project/Project.h"
#include "Blazr/Project/ProjectSerializer.h"
#include <iostream>

namespace Blazr {
class Sandbox : public Application {
  public:
	Sandbox() {
		Renderer2D::Init();
		auto animationSystem =
			std::make_shared<AnimationSystem>(*m_Registry.get());
		auto scriptingSystem =
			std::make_shared<ScriptingSystem>(*m_Registry.get());
		m_Registry->AddToContext(animationSystem);
		m_Registry->AddToContext(scriptingSystem);
		m_Renderer = Renderer2D();
		m_Project =
			ProjectSerializer::Deserialize("project.blzrproj", m_LuaState);
		m_ActiveScene =
			m_Project->GetScene(m_Project->GetConfig().StartSceneName);
	}

	~Sandbox() {}

  private:
	Renderer2D m_Renderer;
	Window::EventCallbackFn m_EventCallback;
	Ref<Project> m_Project;
};

Application *CreateApplication() { return new Sandbox(); }
} // namespace Blazr
