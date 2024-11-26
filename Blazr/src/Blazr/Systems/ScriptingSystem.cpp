#include "blzrpch.h"
#include "../Ecs/Entity.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/AnimationComponent.h"
#include "Blazr/Ecs/Components/BoxColliderComponent.h"
#include "Blazr/Ecs/Components/ScriptComponent.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Blazr/Systems/BoxColliderSystem.h"

#include "Blazr/Ecs/Registry.h"
#include "Blazr/Scripting/GlmLuaBindings.h"
#include "Blazr/Systems/BoxColliderSystem.h"
#include "Blazr/Systems/InputSystem.h"
#include "ScriptingSystem.h"

#include "../Resources/AssetManager.h"
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
	mainLuaScript.AddComponent<Bazr::ScriptComponent>(
		Bazr::ScriptComponent{.update = update, .render = render});

	m_bMainLoaded = true;

	return true;
}
void ScriptingSystem::Update() {
	if (!m_bMainLoaded) {
		BLZR_CORE_ERROR("Main lua script has not been initialized!");
		return;
	}

	auto view = m_Registry.GetRegistry().view<Bazr::ScriptComponent>();

	for (const auto &entity : view) {
		Entity ent{m_Registry, entity};
		if (ent.GetName() != "main_script") {
			continue;
		}

		auto &script = ent.GetComponent<Bazr::ScriptComponent>();
		auto error = script.update(entity);
		if (!error.valid()) {
			sol::error err = error;
			BLZR_CORE_ERROR("Error running the Update script: {0}", err.what());
		}
	}
}
void ScriptingSystem::Render() {
	if (!m_bMainLoaded) {
		BLZR_CORE_ERROR("Main lua script has not been initialized!");
		return;
	}

	auto view = m_Registry.GetRegistry().view<Bazr::ScriptComponent>();

	for (const auto &entity : view) {
		Entity ent{m_Registry, entity};
		if (ent.GetName() != "main_script") {
			continue;
		}

		auto &script = ent.GetComponent<Bazr::ScriptComponent>();
		auto error = script.render(entity);
		if (!error.valid()) {
			sol::error err = error;
			BLZR_CORE_ERROR("Error running the Render script: {0}", err.what());
		}
	}
}

void ScriptingSystem::RegisterLuaBindings(sol::state &lua, Registry &registry) {

	GLMBindings::CreateLuaGLMBinding(lua);
	Registry::CreateLuaRegistryBind(lua, registry);
	BoxColliderSystem::CreateLuaBoxColliderSystemBind(lua);
	InputSystem::CreateInputLuaBind(lua);

	Entity::CreateLuaEntityBind(lua, registry);
	TransformComponent::CreateLuaTransformComponentBind(lua);
	SpriteComponent::CreateLuaSpriteComponentBind(lua, registry);
	BoxColliderComponent::CreateLuaBoxColliderComponentBind(lua);

	Entity::RegisterMetaComponent<TransformComponent>();
	Entity::RegisterMetaComponent<SpriteComponent>();
	Entity::RegisterMetaComponent<BoxColliderComponent>();

	Registry::RegisterMetaComponent<TransformComponent>();
	Registry::RegisterMetaComponent<SpriteComponent>();
	AnimationComponent::CreateAnimationLuaBind(lua);
	BoxColliderComponent::CreateLuaBoxColliderComponentBind(lua);

	Entity::RegisterMetaComponent<TransformComponent>();
	Entity::RegisterMetaComponent<SpriteComponent>();
	Entity::RegisterMetaComponent<AnimationComponent>();
	Entity::RegisterMetaComponent<BoxColliderComponent>();

	Registry::RegisterMetaComponent<TransformComponent>();
	Registry::RegisterMetaComponent<SpriteComponent>();
	Registry::RegisterMetaComponent<AnimationComponent>();
	SoundPlayer::CreateLuaEntityBind(lua);

	Registry::RegisterMetaComponent<BoxColliderComponent>();
	SoundPlayer::CreateLuaEntityBind(lua);
	// AssetManager::CreateLuaEntityBind(lua);
	AssetManager::CreateLuaAssetManager(lua, registry);
	SoundPlayer::CreateLuaSoundPlayer(lua, registry);
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
