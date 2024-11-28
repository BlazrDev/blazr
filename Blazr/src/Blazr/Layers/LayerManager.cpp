#include "blzrpch.h"
#include "LayerManager.h"

namespace Blazr {
Ref<Layer> LayerManager::CreateLayer(const std::string &name, int zIndex) {
	Ref<Layer> layer = CreateRef<Layer>(name, zIndex);
	m_Layers.push_back(layer);
	SortLayers();
	return layer;
}

void LayerManager::RemoveLayer(const std::string &name) {
	m_Layers.erase(std::remove_if(m_Layers.begin(), m_Layers.end(),
								  [&name](Ref<Layer> layer) {
									  return layer->name == name;
								  }),
				   m_Layers.end());
}

void LayerManager::AddEntityToLayer(const std::string &layerName,
									Ref<Entity> entity) {
	Ref<Layer> layer = this->GetLayerByName(layerName);
	layer->AddEntity(entity);
}

void LayerManager::SortLayers() {
	std::sort(m_Layers.begin(), m_Layers.end(),
			  [](Ref<Layer> a, Ref<Layer> b) { return a->zIndex < b->zIndex; });
}

void LayerManager::Render(Registry &registry) {
	int i = 0;
	for (auto layer : m_Layers) {
		layer->Render(registry);
	}
}

Ref<Layer> LayerManager::GetLayerByName(const std::string &name) const {
	auto it = std::find_if(
		m_Layers.begin(), m_Layers.end(),
		[&name](const Ref<Layer> layer) { return layer->name == name; });

	if (it != m_Layers.end()) {
		return *it;
	}

	BLZR_CORE_WARN("Layer with name '{}' does not exist!", name);
	return nullptr;
}

std::vector<Ref<Layer>> LayerManager::GetAllLayers() const { return m_Layers; }

void LayerManager::BindLayerManager(sol::state &lua) {
	lua.new_usertype<LayerManager>(
		"LayerManager", "CreateLayer", &LayerManager::CreateLayer,
		"AddEntityToLayer", &LayerManager::AddEntityToLayer, "GetLayerByName",
		&LayerManager::GetLayerByName);
}

} // namespace Blazr
