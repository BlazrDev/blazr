#pragma once
#include "Blazr/Core/Core.h"
#include "Canvas.h"

class RenderGridGui {

  private:
	Ref<Canvas> m_Canvas;

  public:
	Ref<Canvas> GetCanvas() { return m_Canvas; }

	void SetCanvas(Ref<Canvas> canvas) { m_Canvas = canvas; }
};
