#pragma once
#include "Layer.h"

namespace Blazr {
class LayerManager {
  public:
	Ref<Layer> GetLayerByName(const std::string &name) const;
	Ref<Layer> CreateLayer(const std::string &name, int zIndex = 0);
	void AddEntityToLayer(const std::string &layerName, Ref<Entity> entity);
	void RemoveLayer(const std::string &name);
	void SortLayers();
	void Render(Registry &registry);
	void AddLayer(Ref<Layer> layer);
	std::vector<Ref<Layer>> GetAllLayers() const;

	static void BindLayerManager(sol::state &lua);

  private:
	std::vector<Ref<Layer>> m_Layers = std::vector<Ref<Layer>>();
};
} // namespace Blazr
