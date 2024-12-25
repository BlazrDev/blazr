#pragma once
#include "Layer.h"

namespace Blazr {
class LayerManager {
  public:
	Ref<Layer> BLZR_API GetLayerByName(const std::string &name) const;
	Ref<Layer> BLZR_API CreateLayer(const std::string &name, int zIndex = 0);
	void BLZR_API AddEntityToLayer(const std::string &layerName, Ref<Entity> entity);
	void BLZR_API RemoveLayer(const std::string &name);
	void SortLayers();
	void BLZR_API Render(Registry &registry);
	void BLZR_API AddLayer(Ref<Layer> layer);
	std::vector<Ref<Layer>> BLZR_API GetAllLayers() const;

	static void BindLayerManager(sol::state &lua);

  private:
	std::vector<Ref<Layer>> m_Layers = std::vector<Ref<Layer>>();
};
} // namespace Blazr
