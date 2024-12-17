#include "blzrpch.h"
#include "../Ecs/Entity.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/AnimationComponent.h"
#include "Blazr/Ecs/Components/BoxColliderComponent.h"
#include "Blazr/Ecs/Components/PhysicsComponent.h"
#include "Blazr/Ecs/Components/ScriptComponent.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TileComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Blazr/Systems/BoxColliderSystem.h"

#include "Blazr/Ecs/Registry.h"
#include "Blazr/Scripting/GlmLuaBindings.h"
#include "Blazr/Systems/BoxColliderSystem.h"
#include "Blazr/Systems/InputSystem.h"
#include "Blazr/Systems/PhysicsSystem.h"
#include "ScriptingSystem.h"

#include "../Resources/AssetManager.h"
#include "Blazr/Core/Log.h"
#include "Sounds/SoundPlayer.h"
namespace Blazr {
ScriptingSystem::ScriptingSystem(Registry &registry) : m_Registry(registry) {}

bool ScriptingSystem::LoadMainScript(sol::state &lua) {

	try {
		auto result = lua.safe_script_file("assets/scripts/main.lua");
	} catch (const sol::error &err) {
		BLZR_CORE_ERROR("Error loading main lua file! {0}", err.what());
		return false;
	}

	sol::table main_lua = lua["main"];
	sol::optional<sol::table> bUpdateExists = main_lua[1];
	if (bUpdateExists == sol::nullopt) {
		BLZR_CORE_ERROR("There is no update funciton in main.lua");
		return false;
	}

	sol::table update_script = main_lua[1];
	sol::function update = update_script["update"];

	sol::optional<sol::table> bRenderExists = main_lua[2];
	if (bRenderExists == sol::nullopt) {
		BLZR_CORE_ERROR("There is no render funciton in main.lua");
		return false;
	}

	sol::table render_script = main_lua[2];
	sol::function render = render_script["render"];

	Entity mainLuaScript(m_Registry, "main_script", "");
	mainLuaScript.AddComponent<Blazr::ScriptComponent>(
		Blazr::ScriptComponent{.update = update, .render = render});

	m_bMainLoaded = true;

	return true;
}

void ScriptingSystem::Update() {
	auto view = m_Registry.GetRegistry().view<Blazr::ScriptComponent>();

	for (const auto &entity : view) {
		Entity ent{m_Registry, entity};

		auto &script = ent.GetComponent<Blazr::ScriptComponent>();
		if (script.update.valid()) {
			auto result = script.update(entity);
			if (!result.valid()) {
				sol::error err = result;
				BLZR_CORE_ERROR(
					"Error running Update script for entity '{}': {}",
					ent.GetName(), err.what());
			}
		} else {
			BLZR_CORE_WARN(
				"Entity '{}' has no valid update function in its script.",
				ent.GetName());
		}
	}
}

void ScriptingSystem::Render() {
	auto view = m_Registry.GetRegistry().view<Blazr::ScriptComponent>();

	for (const auto &entity : view) {
		Entity ent{m_Registry, entity};

		auto &script = ent.GetComponent<Blazr::ScriptComponent>();
		if (script.render.valid()) {
			auto result = script.render(entity);
			if (!result.valid()) {
				sol::error err = result;
				BLZR_CORE_ERROR(
					"Error running Render script for entity '{}': {}",
					ent.GetName(), err.what());
			}
		} else {
			BLZR_CORE_WARN(
				"Entity '{}' has no valid render function in its script.",
				ent.GetName());
		}
	}
}

void ScriptingSystem::InitializeEntityScripts(sol::state &lua) {
	auto view = m_Registry.GetRegistry().view<Blazr::ScriptComponent>();

	for (const auto &entity : view) {
		Entity ent{m_Registry, entity};
		auto &script = ent.GetComponent<Blazr::ScriptComponent>();

		try {
			sol::table scriptTable =
				lua.require_file(ent.GetName(), script.scriptPath);

			if (scriptTable["on_update"].valid()) {
				script.update = scriptTable["on_update"];
			} else {
				BLZR_CORE_WARN(
					"Script '{}' for entity '{}' has no 'on_update' function.",
					script.scriptPath, ent.GetName());
			}

			if (scriptTable["on_render"].valid()) {
				script.render = scriptTable["on_render"];
			} else {
				BLZR_CORE_WARN(
					"Script '{}' for entity '{}' has no 'on_render' function.",
					script.scriptPath, ent.GetName());
			}
		} catch (const sol::error &err) {
			BLZR_CORE_ERROR("Failed to load script '{}' for entity '{}': {}",
							script.scriptPath, ent.GetName(), err.what());
		}
	}
}

void ScriptingSystem::RegisterLuaBindings(sol::state &lua, Registry &registry) {

	GLMBindings::CreateLuaGLMBinding(lua);
	Registry::CreateLuaRegistryBind(lua, registry);
	InputSystem::CreateInputLuaBind(lua);
	BoxColliderSystem::CreateLuaBoxColliderSystemBind(lua);

	Entity::CreateLuaEntityBind(lua, registry);
	TransformComponent::CreateLuaTransformComponentBind(lua);
	SpriteComponent::CreateLuaSpriteComponentBind(lua, registry);
	BoxColliderComponent::CreateLuaBoxColliderComponentBind(lua);
	PhysicsComponent::CreateLuaPhysicsComponentBind(lua, registry);
	AnimationComponent::CreateAnimationLuaBind(lua);
	TileComponent::CreateLuaTileComponentBind(lua);

	Entity::RegisterMetaComponent<TransformComponent>();
	Entity::RegisterMetaComponent<SpriteComponent>();
	Entity::RegisterMetaComponent<BoxColliderComponent>();
	Entity::RegisterMetaComponent<AnimationComponent>();
	Entity::RegisterMetaComponent<PhysicsComponent>();
	Entity::RegisterMetaComponent<TileComponent>();

	Registry::RegisterMetaComponent<TransformComponent>();
	Registry::RegisterMetaComponent<SpriteComponent>();
	Registry::RegisterMetaComponent<AnimationComponent>();
	Registry::RegisterMetaComponent<BoxColliderComponent>();
	Registry::RegisterMetaComponent<PhysicsComponent>();
	Registry::RegisterMetaComponent<TileComponent>();
	SoundPlayer::CreateLuaEntityBind(lua);

	Registry::RegisterMetaComponent<BoxColliderComponent>();
	SoundPlayer::CreateLuaEntityBind(lua);
	// AssetManager::CreateLuaEntityBind(lua);
	AssetManager::CreateLuaAssetManager(lua, registry);
	SoundPlayer::CreateLuaSoundPlayer(lua, registry);
	Log::createLogLuaBind(lua);
}

void ScriptingSystem::RegisterLuaFunctions(sol::state &lua) {
	lua.set_function("run_script", [&](const std::string &path) {
		try {
			lua.safe_script_file(path);
		} catch (sol::error &error) {
			BLZR_CORE_ERROR("Failed to load Lua script {0}", error.what());
			return false;
		}
		return true;
	});
}

} // namespace Blazr
