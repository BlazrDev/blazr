#pragma once

#include "Blazr/Core/Core.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/TileComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Blazr/Ecs/Entity.h"
#include "Blazr/Renderer/Canvas.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Resources/AssetManager.h"
#include "Blazr/Scene/Scene.h"
#include "imgui.h"

namespace Blazr {
class TilemapScene : public Blazr::Scene {

  private:
	Ref<Canvas> m_Canvas;
	std::pair<std::string, glm::vec3> m_selectedTile = {"", {0, 0, 0}};
	bool m_ShowGrid = true;

  public:
	TilemapScene(Canvas &canvas) : Scene() {
		this->SetName("TileMapScene");
		m_Canvas = CreateRef<Canvas>(canvas);
		m_LayerManager->CreateLayer("Grid", 0);
		m_LayerManager->CreateLayer("Tilemap", 1);

		float tileSize = m_Canvas->GetTileSize();
		glm::vec2 position = {1280, 720};
		glm::vec2 centerPosition{0.f};

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
				Entity entity = Entity(*m_Registry, "", "grid");

				auto &transform =
					entity.AddComponent<TransformComponent>(TransformComponent{
						.position = {centerPosition.x + x * tileSize,
									 centerPosition.y + y * tileSize}});

				auto &sprite = entity.AddComponent<SpriteComponent>(
					SpriteComponent{.width = tileSize,
									.height = tileSize,
									.color = color,
									.layer = "Grid"});

				auto &tile = entity.AddComponent<TileComponent>(
					TileComponent{.name = "grid"});
				m_LayerManager->AddEntityToLayer("Grid",
												 CreateRef<Entity>(entity));
			}
		}

		// if (mousePosition.x < 0)
		// 	mousePosition.x = 0;
		// if (mousePosition.y < 0)
		// 	mousePosition.y = 0;

		// Izračunavamo relativnu poziciju miša prema mreži
	}

	std::pair<std::string, glm::vec3> GetSelectedTile() {
		return m_selectedTile;
	}
	void SetSelectedTile(std::pair<std::string, glm::vec3> texture) {
		m_selectedTile = texture;
	}

	void Render() override {

		Blazr::Renderer2D::BeginScene(m_Camera.GetCamera());
		m_Camera.GetCamera().Update();
		m_Camera.OnUpdate();
		float tileSize = m_Canvas->GetTileSize();
		glm::vec2 position = {1280, 720};
		glm::vec2 centerPosition{0.f};
		float zoom = m_Camera.GetCamera().GetScale();
		glm::vec2 mousePosition = {(m_Camera.GetMousePosition().x +
									m_Camera.GetCamera().GetPosition().x) /
									   zoom,
								   (ImGui::GetWindowSize().y -
									m_Camera.GetMousePosition().y +
									m_Camera.GetCamera().GetPosition().y) /
									   zoom};

		float gridX = static_cast<int>(mousePosition.x / tileSize);
		float gridY = static_cast<int>(mousePosition.y / tileSize);

		// Proveravamo da li je miš u granicama mreže
		if (gridX >= 0 && gridX < m_Canvas->GetWidth() && gridY >= 0 &&
			gridY < m_Canvas->GetHeight()) {

			// Dodatna logika za klikove ili označavanje kvadrata
			glm::vec4 highlightColor = {255.0f, 0.0f, 0.0f, 1.0f};

			if (m_selectedTile.first != "") {

				if (ImGui::IsWindowHovered() &&
					ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					BLZR_CORE_INFO("Mouse clicked");

					Entity entity = Entity(*m_Registry, "", "tilemap");

					auto &transform = entity.AddComponent<TransformComponent>(
						TransformComponent{
							.position = {centerPosition.x + gridX * tileSize,
										 centerPosition.y + gridY * tileSize}});

					auto &sprite =
						entity.AddComponent<SpriteComponent>(SpriteComponent{
							.width = tileSize,
							.height = tileSize,
							.startX = static_cast<int>(m_selectedTile.second.x),
							.startY =
								static_cast<int>(m_selectedTile.second.y) -
								static_cast<int>(m_selectedTile.second.z) - 1,
							.layer = "Tilemap",
							.texturePath = m_selectedTile.first});

					auto &tile = entity.AddComponent<TileComponent>(
						TileComponent{.name = m_selectedTile.first});
					auto texture = AssetManager::GetInstance()->GetTexture(
						m_selectedTile.first);
					if (texture == nullptr) {
						BLZR_CORE_ERROR("Texture is null {0}",
										m_selectedTile.first);
					}
					sprite.generateObject(texture->GetWidth(),
										  texture->GetHeight());

					m_LayerManager->AddEntityToLayer("Tilemap",
													 CreateRef<Entity>(entity));
				}

			} else {

				Blazr::Renderer2D::DrawQuad(
					entt::null,
					{centerPosition.x + gridX * tileSize,
					 centerPosition.y + gridY * tileSize},
					{tileSize, tileSize}, highlightColor);
			}
		}

		m_LayerManager->Render(*m_Registry);
		Blazr::Renderer2D::EndScene();
		Blazr::Renderer2D::Flush();
	}

	Ref<Canvas> GetCanvas() { return m_Canvas; }
	void SetCanvas(Ref<Canvas> canvas) { m_Canvas = canvas; }
};
} // namespace Blazr
