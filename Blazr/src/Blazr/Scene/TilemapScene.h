#pragma once

#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Blazr/Renderer/Canvas.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Scene/Scene.h"
#include "imgui.h"

namespace Blazr {
class TilemapScene : public Blazr::Scene {

  private:
	Ref<Canvas> m_Canvas;

  public:
	TilemapScene(Canvas &canvas) : Scene() {
		this->SetName("TileMapScene");
		m_Canvas = CreateRef<Canvas>(canvas);
		m_LayerManager->CreateLayer("Tilemap", 0);
	}

	void Render() override {

		Blazr::Renderer2D::BeginScene(m_Camera.GetCamera());
		m_Camera.GetCamera().Update();
		m_Camera.OnUpdate();
		float tileSize = m_Canvas->GetTileSize();
		// glm::vec2 position = {ImGui::GetWindowSize().x,
		// 					  ImGui::GetWindowSize().y};

		// BLZR_CORE_INFO("Position: {0} {1}", position.x, position.y);
		glm::vec2 position = {1280, 720};
		glm::vec2 centerPosition{0.f};
		// glm::vec2 centerPosition = {
		// 	(position.x / 2 - m_Canvas->GetWidth() / 2),
		// 	(position.y / 2 - m_Canvas->GetHeight() / 2)};

		BLZR_CORE_INFO("Center Position: {0} {1}", centerPosition.x,
					   centerPosition.y);

		glm::vec4 color = {0.f, 0.f, 0.f, 1.f};
		for (int x = 0; x < m_Canvas->GetWidth() / m_Canvas->GetTileSize();
			 x++) {
			for (int y = 0; y < m_Canvas->GetHeight() / m_Canvas->GetTileSize();
				 y++) {

				if ((y - x) % 2 != 0 && (x - y) % 2 != 0) {
					color = {255.f, 255.f, 255.f, 0.1f};
				} else {
					color = {255.f, 255.f, 255.f, 0.4f};
				}
				Blazr::Renderer2D::DrawQuad(entt::null,
											{centerPosition.x + x * tileSize,
											 centerPosition.y + y * tileSize},
											{tileSize, tileSize}, color);
			}
		}

		float zoom = m_Camera.GetCamera().GetScale();
		glm::vec2 mousePosition = {(m_Camera.GetMousePosition().x +
									m_Camera.GetCamera().GetPosition().x) /
									   zoom,
								   (ImGui::GetWindowSize().y -
									m_Camera.GetMousePosition().y +
									m_Camera.GetCamera().GetPosition().y) /
									   zoom};

		// if (mousePosition.x < 0)
		// 	mousePosition.x = 0;
		// if (mousePosition.y < 0)
		// 	mousePosition.y = 0;

		BLZR_CORE_ERROR("MIS: {0} {1} {2}", mousePosition.x, mousePosition.y,
						zoom);
		// Izračunavamo relativnu poziciju miša prema mreži
		float gridX = static_cast<int>(mousePosition.x / tileSize);
		float gridY = static_cast<int>(mousePosition.y / tileSize);

		BLZR_CORE_ERROR("CAMERA: {0} {1}", m_Camera.GetCamera().GetPosition().x,
						m_Camera.GetCamera().GetPosition().y);

		// Proveravamo da li je miš u granicama mreže
		if (gridX >= 0 && gridX < m_Canvas->GetWidth() && gridY >= 0 &&
			gridY < m_Canvas->GetHeight()) {
			std::cout << "Mouse is over tile: (" << gridX << ", " << gridY
					  << ")" << std::endl;

			// Dodatna logika za klikove ili označavanje kvadrata
			glm::vec4 highlightColor = {255.0f, 0.0f, 0.0f, 1.0f};
			Blazr::Renderer2D::DrawQuad(entt::null,
										{centerPosition.x + gridX * tileSize,
										 centerPosition.y + gridY * tileSize},
										{tileSize, tileSize}, highlightColor);
		} else {
			std::cout << "Mouse is outside the grid." << std::endl;
		}

		Blazr::Renderer2D::EndScene();
		Blazr::Renderer2D::Flush();
	}

	Ref<Canvas> GetCanvas() { return m_Canvas; }
	void SetCanvas(Ref<Canvas> canvas) { m_Canvas = canvas; }
};
} // namespace Blazr
