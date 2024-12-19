#pragma once

#include "Blazr/Core/Core.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/BoxColliderComponent.h"
#include "Blazr/Ecs/Components/Identification.h"
#include "Blazr/Ecs/Components/PhysicsComponent.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TileComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Blazr/Ecs/Entity.h"
#include "Blazr/Renderer/Canvas.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Resources/AssetManager.h"
#include "Blazr/Scene/Scene.h"
#include "imgui.h"
#include <string>

namespace Blazr {
class TilemapScene : public Blazr::Scene {

  private:
	Ref<Canvas> m_Canvas;
	std::pair<std::string, glm::vec3> m_selectedTile = {"", {0, 0, 0}};
	Entity m_SelectedEntity{*m_Registry, "selected", "selected"};
	bool isShiftPressed = false;
	std::string layer = "";
	glm::vec2 scale{1.0f, 1.0f};

  public:
	TilemapScene(Canvas &canvas) : Scene() {
		this->SetName("TileMapScene");
		m_Canvas = CreateRef<Canvas>(canvas);
		m_LayerManager->CreateLayer("Grid", 0);
		m_LayerManager->CreateLayer("Tilemap", 1);
		m_LayerManager->CreateLayer("Objects", 2);
		m_LayerManager->CreateLayer("Collider", 10);

		initCanvas();
		m_SelectedEntity.AddComponent<TransformComponent>();
		m_SelectedEntity.AddComponent<SpriteComponent>();
		m_SelectedEntity.AddComponent<TileComponent>();
	}

	void SetScale(glm::vec2 scale) { this->scale = scale; }

	glm::vec2 GetScale() { return scale; }

	void initCanvas() {
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
	}

	void removeGrid() {
		auto entities = m_LayerManager->GetLayerByName("Grid")->GetEntities();
		for (auto entity : entities) {
			m_LayerManager->GetLayerByName("Grid")->RemoveEntity(entity);
		}
	}

	std::pair<std::string, glm::vec3> GetSelectedTile() {
		return m_selectedTile;
	}

	std::string GetLayer() { return layer; }

	void SetLayer(std::string layer) { this->layer = layer; }
	void SetSelectedTile(std::pair<std::string, glm::vec3> texture) {
		m_selectedTile = texture;

		m_SelectedEntity.ReplaceComponent<TransformComponent>(
			TransformComponent{.position = {0, 0}});
		auto &sprite =
			m_SelectedEntity.ReplaceComponent<SpriteComponent>(SpriteComponent{
				.width = static_cast<float>(m_Canvas->GetTileSize()),
				.height = static_cast<float>(m_Canvas->GetTileSize()),
				.startX = static_cast<int>(texture.second.x),
				.startY = static_cast<int>(texture.second.y) -
						  static_cast<int>(texture.second.z) - 1,
				.texturePath = texture.first});

		m_SelectedEntity.ReplaceComponent<TileComponent>(
			TileComponent{.name = texture.first});

		auto t = AssetManager::GetInstance()->GetTexture(m_selectedTile.first);

		sprite.generateObject(t->GetWidth(), t->GetHeight());
	}

	void Render() override {

		Blazr::Renderer2D::BeginScene(m_Camera.GetCamera());
		m_Camera.GetCamera().Update();
		m_Camera.OnUpdate();

		if (m_Canvas->GetUpdate()) {
			removeGrid();
			initCanvas();
			m_Canvas->SetUpdate(false);
		}
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
		if (ImGui::GetIO().KeyShift) {
			isShiftPressed = true;
		} else {
			isShiftPressed = false;
		}

		// Proveravamo da li je miš u granicama mreže
		if (gridX >= 0 &&
			gridX < m_Canvas->GetWidth() / m_Canvas->GetTileSize() &&
			gridY >= 0 &&
			gridY < m_Canvas->GetHeight() / m_Canvas->GetTileSize()) {

			// Dodatna logika za klikove ili označavanje kvadrata
			glm::vec4 highlightColor = {255.0f, 0.0f, 0.0f, 1.0f};

			if (m_selectedTile.first != "") {
				auto texture = AssetManager::GetInstance()->GetTexture(
					m_selectedTile.first);
				if (texture == nullptr) {
					BLZR_CORE_ERROR("Texture is null {0}",
									m_selectedTile.first);
				}

				Renderer2D::DrawQuad(*m_Registry,
									 m_SelectedEntity.GetEntityHandler());
				m_SelectedEntity.ReplaceComponent<TransformComponent>(
					TransformComponent{
						.position = {centerPosition.x + gridX * tileSize,
									 centerPosition.y + gridY * tileSize}});

				if (ImGui::IsWindowHovered() &&
					ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
					!isShiftPressed) {

					if (!IsEntityAtPosition(m_Registry.get(),
											centerPosition.x + gridX * tileSize,
											centerPosition.y + gridY * tileSize,
											layer) &&
						m_selectedTile.first != "collider") {

						addTile(std::to_string(gridX) + "  " +
									std::to_string(gridY),
								centerPosition.x + gridX * tileSize,
								centerPosition.y + gridY * tileSize, tileSize);
					} else if (!IsEntityAtPosition(
								   m_Registry.get(),
								   centerPosition.x + gridX * tileSize,
								   centerPosition.y + gridY * tileSize,
								   layer) &&
							   m_selectedTile.first == "collider") {
						addCollider("Collider: " + std::to_string(gridX) +
										"  " + std::to_string(gridY),
									centerPosition.x + gridX * tileSize,
									centerPosition.y + gridY * tileSize,
									tileSize);
					}
				} else {
					if (ImGui::IsWindowHovered() &&
						ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
						isShiftPressed) {
						auto view =
							m_Registry->GetRegistry()
								.view<TransformComponent, SpriteComponent,
									  Identification>();
						for (auto entity : view) {
							auto &transform =
								view.get<TransformComponent>(entity);
							auto &sprite = view.get<SpriteComponent>(entity);
							auto &id = view.get<Identification>(entity);
							if (transform.position.x ==
									centerPosition.x + gridX * tileSize &&
								transform.position.y ==
									centerPosition.y + gridY * tileSize &&
								sprite.layer == layer) {
								m_LayerManager->GetLayerByName(layer)
									->RemoveEntity(id.name);
								m_Registry->GetRegistry().destroy(entity);
							}
						}
					}
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

	void addTile(const std::string &name, int x, int y, int tileSize = 16) {
		auto texture =
			AssetManager::GetInstance()->GetTexture(m_selectedTile.first);

		Entity entity = Entity(*m_Registry, name, "tilemap");

		auto &transform = entity.AddComponent<TransformComponent>(
			TransformComponent{.position = {x, y}});

		auto &sprite = entity.AddComponent<SpriteComponent>(SpriteComponent{
			.width = static_cast<float>(tileSize),
			.height = static_cast<float>(tileSize),
			.startX = static_cast<int>(m_selectedTile.second.x),
			.startY = static_cast<int>(m_selectedTile.second.y) -
					  static_cast<int>(m_selectedTile.second.z) - 1,
			.layer = layer,
			.texturePath = m_selectedTile.first});

		auto &tile = entity.AddComponent<TileComponent>(
			TileComponent{.name = m_selectedTile.first});

		sprite.generateObject(texture->GetWidth(), texture->GetHeight());
		m_LayerManager->AddEntityToLayer(layer, CreateRef<Entity>(entity));
	}

	void addCollider(const std::string &name, int x, int y, int tileSize = 16) {
		auto texture =
			AssetManager::GetInstance()->GetTexture(m_selectedTile.first);

		Entity entity = Entity(*m_Registry, name, "tilemap");

		auto &transform = entity.AddComponent<TransformComponent>(
			TransformComponent{.position = {x, y}});

		auto &sprite = entity.AddComponent<SpriteComponent>(SpriteComponent{
			.width = static_cast<float>(tileSize),
			.height = static_cast<float>(tileSize),
			.startX = static_cast<int>(m_selectedTile.second.x),
			.startY = static_cast<int>(m_selectedTile.second.y) -
					  static_cast<int>(m_selectedTile.second.z) - 1,
			.layer = layer,
			.texturePath = m_selectedTile.first});

		auto &tile = entity.AddComponent<TileComponent>(
			TileComponent{.name = m_selectedTile.first});
		auto &collider = entity.AddComponent<BoxColliderComponent>(
			BoxColliderComponent{.width = static_cast<int>(tileSize),
								 .height = static_cast<int>(tileSize)});
		sprite.generateObject(texture->GetWidth(), texture->GetHeight());

		auto &physicsWorld = m_Registry->GetContext<std::shared_ptr<b2World>>();

		if (!physicsWorld) {
			return;
		}

		PhysicsAttributes attributes{
			.friction = 100,
			.position = transform.position,
			.scale = transform.scale,
			.boxSize = {collider.width, collider.height},
			.offset = collider.offset,
		};
		entity.AddComponent<PhysicsComponent>(
			PhysicsComponent(physicsWorld, attributes));

		m_LayerManager->AddEntityToLayer(layer, CreateRef<Entity>(entity));
	}
	bool IsEntityAtPosition(Blazr::Registry *registry, float posX, float posY,
							const std::string &layer) {
		auto view =
			registry->GetRegistry().view<TransformComponent, SpriteComponent>();
		for (auto entity : view) {
			auto &transform = view.get<TransformComponent>(entity);
			auto &sprite = view.get<SpriteComponent>(entity);

			if (transform.position.x == posX && transform.position.y == posY &&
				sprite.layer == layer) {
				return true;
			}
		}
		return false; // Nema entiteta na toj poziciji sa istim slojem
	}

	void ExportTilemapToScene(Scene &scene) {

		clearScene(scene);

		auto layerManager = scene.GetLayerManager();
		for (auto layer : m_LayerManager->GetAllLayers()) {
			if (layer->name == "Grid") {
				continue;
			}
			auto sceneLayerManager = layerManager->GetLayerByName(layer->name);
			if (sceneLayerManager == nullptr) {
				sceneLayerManager =
					layerManager->CreateLayer(layer->name, layer->zIndex);
			}
			for (auto entity : layer->GetEntities()) {
				sceneLayerManager->AddEntity(entity);
			}
		}
		// clearScene();
	}

	void clearScene(Scene &scene) {

		auto layerManager = scene.GetLayerManager();
		for (auto layer : m_LayerManager->GetAllLayers()) {
			if (layer->name != "Grid") {
				auto sceneLayer = layerManager->GetLayerByName(layer->name);
				if (sceneLayer == nullptr) {
					continue;
				}

				for (auto entity : sceneLayer->GetEntities()) {
					sceneLayer->RemoveEntity(entity);
				}
			}
		}
	}

	void clearRegistry() {
		for (auto layer : m_LayerManager->GetAllLayers()) {
			if (layer->name != "Grid") {
				for (auto entity : layer->GetEntities()) {
					m_LayerManager->GetLayerByName(layer->name)
						->RemoveEntity(entity);
				}
			}
		}
	}

	Ref<Canvas> GetCanvas() { return m_Canvas; }
	void SetCanvas(Ref<Canvas> canvas) {
		m_Canvas = canvas;
		initCanvas();
	}
};
} // namespace Blazr
