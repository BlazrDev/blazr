#pragma once

#include "Blazr/Ecs/Entity.h"
#include "Blazr/Ecs/Registry.h"
#include "Blazr/Layers/LayerManager.h"
#include "Blazr/Renderer/Camera2D.h"
#include "Blazr/Renderer/CameraController.h"
#include "Blazr/Renderer/FrameBuffer.h"
#include "Blazr/Renderer/Texture2D.h"
#include <json.hpp>
#include <memory>
#include <vector>

using namespace nlohmann;

namespace Blazr {

class Scene {
  public:
	BLZR_API Scene();
	BLZR_API ~Scene();

	entt::entity AddEntity();
	void RemoveEntity(entt::entity entity);

	std::shared_ptr<Registry> GetRegistry() { return m_Registry; }
	CameraController &GetCameraController() { return m_Camera; }

	void BLZR_API Update();
	virtual void BLZR_API Render();

	void BLZR_API onEvent(Event &e);

	void Serialize(json &j) const;
	void Deserialize(const json &j);

	void AddLayer(const std::string &layerName, int zIndex);
	void RemoveLayer(const std::string &layerName);
	Ref<Layer> BLZR_API GetLayerByName(const std::string &layerName);
	std::vector<Ref<Layer>> GetAllLayers() const;

	void AddEntityToLayer(const std::string &layerName, Ref<Entity> entity);
	void RemoveEntityFromLayer(const std::string &layerName,
							   Ref<Entity> entity);

	Ref<LayerManager> GetLayerManager() { return m_LayerManager; }

	static void BindScene(sol::state &lua);

	void SetName(const std::string &name) { m_Name = name; }
	std::string GetName() const { return m_Name; }
	Ref<FrameBuffer> m_GameFrameBuffer;

  protected:
	std::string m_Name = "Untitled";
	CameraController m_Camera;
	std::shared_ptr<Blazr::Registry> m_Registry;
	Ref<LayerManager> m_LayerManager;
};

} // namespace Blazr
