#include "blzrpch.h"
#include "../Ecs/Entity.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/ScriptComponent.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"

#include "ScriptingSystem.h"
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

	Registry::CreateLuaRegistryBind(lua, registry);

	Entity::CreateLuaEntityBind(lua, registry);
	TransformComponent::CreateLuaTransformComponentBind(lua);
	SpriteComponent::CreateLuaSpriteComponentBind(lua, registry);

	Entity::RegisterMetaComponent<TransformComponent>();
	Entity::RegisterMetaComponent<SpriteComponent>();

	Registry::RegisterMetaComponent<TransformComponent>();
	Registry::RegisterMetaComponent<SpriteComponent>();
}

} // namespace Blazr
