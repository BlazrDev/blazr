#include "blzrpch.h"
#include "GlmLuaBindings.h"
#include "cmath"
#include "glm.hpp"
#include "sol.hpp"
#include <algorithm>

// glm::vec2
void CreateVec2Bind(sol::state &lua) {
	auto vec2MultiplyOverloads = sol::overload(
		[](const glm::vec2 &v1, const glm::vec2 &v2) { return v1 * v2; },
		[](const glm::vec2 &v1, float scalar) { return v1 * scalar; },
		[](float scalar, const glm::vec2 &v1) { return v1 * scalar; });

	auto vec2DevideOverloads = sol::overload(
		[](const glm::vec2 &v1, const glm::vec2 &v2) { return v1 / v2; },
		[](const glm::vec2 &v1, float scalar) { return v1 / scalar; },
		[](float scalar, const glm::vec2 &v1) { return v1 / scalar; });

	auto vec2AdditionOverloads = sol::overload(
		[](const glm::vec2 &v1, const glm::vec2 &v2) { return v1 + v2; },
		[](const glm::vec2 &v1, float scalar) { return v1 + scalar; },
		[](float scalar, const glm::vec2 &v1) { return v1 + scalar; });

	auto vec2SubtractionOverloads = sol::overload(
		[](const glm::vec2 &v1, const glm::vec2 &v2) { return v1 - v2; },
		[](const glm::vec2 &v1, float scalar) { return v1 - scalar; },
		[](float scalar, const glm::vec2 &v1) { return v1 - scalar; });

	// crate vec2 usertype
	lua.new_usertype<glm::vec2>(
		"vec2", sol::call_constructor,
		sol::constructors<glm::vec2(float), glm::vec2(float, float)>(), "x",
		&glm::vec2::x, "y", &glm::vec2::y, sol::meta_function::multiplication,
		vec2MultiplyOverloads, sol::meta_function::division,
		vec2DevideOverloads, sol::meta_function::addition,
		vec2AdditionOverloads, sol::meta_function::subtraction,
		vec2SubtractionOverloads);
}

// glm::vec3
void CreateVec3Bind(sol::state &lua) {
	auto vec3MultiplyOverloads = sol::overload(
		[](const glm::vec3 &v1, const glm::vec3 &v2) { return v1 * v2; },
		[](const glm::vec3 &v1, float scalar) { return v1 * scalar; },
		[](float scalar, const glm::vec3 &v1) { return v1 * scalar; });

	auto vec3DevideOverloads = sol::overload(
		[](const glm::vec3 &v1, const glm::vec3 &v2) { return v1 / v2; },
		[](const glm::vec3 &v1, float scalar) { return v1 / scalar; },
		[](float scalar, const glm::vec3 &v1) { return v1 / scalar; });

	auto vec3AdditionOverloads = sol::overload(
		[](const glm::vec3 &v1, const glm::vec3 &v2) { return v1 + v2; },
		[](const glm::vec3 &v1, float scalar) { return v1 + scalar; },
		[](float scalar, const glm::vec3 &v1) { return v1 + scalar; });

	auto vec3SubtractionOverloads = sol::overload(
		[](const glm::vec3 &v1, const glm::vec3 &v2) { return v1 - v2; },
		[](const glm::vec3 &v1, float scalar) { return v1 - scalar; },
		[](float scalar, const glm::vec3 &v1) { return v1 - scalar; });

	// crate vec3 usertype
	lua.new_usertype<glm::vec3>(
		"vec3", sol::call_constructor,
		sol::constructors<glm::vec3(float), glm::vec3(float, float, float)>(),
		"x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z,
		sol::meta_function::multiplication, vec3MultiplyOverloads,
		sol::meta_function::division, vec3DevideOverloads,
		sol::meta_function::addition, vec3AdditionOverloads,
		sol::meta_function::subtraction, vec3SubtractionOverloads);
}

// glm::vec4
void Createvec4Bind(sol::state &lua) {
	auto vec4MultiplyOverloads = sol::overload(
		[](const glm::vec4 &v1, const glm::vec4 &v2) { return v1 * v2; },
		[](const glm::vec4 &v1, float scalar) { return v1 * scalar; },
		[](float scalar, const glm::vec4 &v1) { return v1 * scalar; });

	auto vec4DevideOverloads = sol::overload(
		[](const glm::vec4 &v1, const glm::vec4 &v2) { return v1 / v2; },
		[](const glm::vec4 &v1, float scalar) { return v1 / scalar; },
		[](float scalar, const glm::vec4 &v1) { return v1 / scalar; });

	auto vec4AdditionOverloads = sol::overload(
		[](const glm::vec4 &v1, const glm::vec4 &v2) { return v1 + v2; },
		[](const glm::vec4 &v1, float scalar) { return v1 + scalar; },
		[](float scalar, const glm::vec4 &v1) { return v1 + scalar; });

	auto vec4SubtractionOverloads = sol::overload(
		[](const glm::vec4 &v1, const glm::vec4 &v2) { return v1 - v2; },
		[](const glm::vec4 &v1, float scalar) { return v1 - scalar; },
		[](float scalar, const glm::vec4 &v1) { return v1 - scalar; });

	// crate vec4 usertype
	lua.new_usertype<glm::vec4>(
		"vec4", sol::call_constructor,
		sol::constructors<glm::vec4(float),
						  glm::vec4(float, float, float, float)>(),
		"x", &glm::vec4::x, "y", &glm::vec4::y, "z", &glm::vec4::z, "w",
		&glm::vec4::w, sol::meta_function::multiplication,
		vec4MultiplyOverloads, sol::meta_function::division,
		vec4DevideOverloads, sol::meta_function::addition,
		vec4AdditionOverloads, sol::meta_function::subtraction,
		vec4SubtractionOverloads);
}

void MathFreeFunction(sol::state &lua) {
	lua.set_function(
		"distance",
		sol::overload(
			[](glm::vec2 &a, glm::vec2 &b) { return glm::distance(a, b); },
			[](glm::vec3 &a, glm::vec3 &b) { return glm::distance(a, b); },
			[](glm::vec4 &a, glm::vec4 &b) { return glm::distance(a, b); }));

	lua.set_function(
		"lerp", [](float a, float b, float t) { return std::lerp(a, b, t); });

	lua.set_function("clamp", sol::overload(
								  [](float value, float min, float max) {
									  return std::clamp(value, min, max);
								  },
								  [](double value, double min, double max) {
									  return std::clamp(value, min, max);
								  },
								  [](int value, int min, int max) {
									  return std::clamp(value, min, max);
								  }

								  ));
}

void Blazr::GLMBindings::CreateLuaGLMBinding(sol::state &lua) {
	CreateVec2Bind(lua);
	CreateVec3Bind(lua);
	Createvec4Bind(lua);

	MathFreeFunction(lua);
}
