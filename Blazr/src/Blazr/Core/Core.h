#pragma once

#ifdef BLZR_PLATFORM_WINDOWS
#ifdef BLZR_BUILD_DLL
#define BLZR_API __declspec(dllexport)
#else
#define BLZR_API __declspec(dllimport)
#endif
#elif BLZR_PLATFORM_LINUX
#define BLZR_API __attribute__((visibility("default")))
#else
#error Blazer only supports Linux and Windows
#endif

#define BIT(x) (1 << x)
#include <memory>

template <typename T> using Scope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr Scope<T> CreateScope(Args &&...args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T> using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Ref<T> CreateRef(Args &&...args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}
