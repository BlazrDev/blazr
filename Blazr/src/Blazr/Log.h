#pragma once
#include "Core.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace Blazr {
class BLZR_API Log {
public:
  static void init();

  inline static std::shared_ptr<spdlog::logger> &getCoreLogger() {
    return s_CoreLogger;
  }
  inline static std::shared_ptr<spdlog::logger> &getClientLogger() {
    return s_ClientLogger;
  }

private:
  static std::shared_ptr<spdlog::logger> s_CoreLogger;
  static std::shared_ptr<spdlog::logger> s_ClientLogger;
};
} // namespace Blazr

// CORE LOG MACROS
#define BLZR_CORE_TRACE(...) ::Blazr::Log::getCoreLogger()->trace(__VA_ARGS__)
#define BLZR_CORE_INFO(...) ::Blazr::Log::getCoreLogger()->info(__VA_ARGS__)
#define BLZR_CORE_WARN(...) ::Blazr::Log::getCoreLogger()->warn(__VA_ARGS__)
#define BLZR_CORE_ERROR(...) ::Blazr::Log::getCoreLogger()->error(__VA_ARGS__)
#define BLZR_CORE_FATAL(...) ::Blazr::Log::getCoreLogger()->fatal(__VA_ARGS__)
#define BLZR_CORE_CRITICAL(...)                                                \
  ::Blazr::Log::getCoreLogger()->critical(__VA_ARGS__)

// CLIENT LOG MCROS
#define BLZR_CLIENT_TRACE(...)                                                 \
  ::Blazr::Log::getClientLogger()->trace(__VA_ARGS__)
#define BLZR_CLIENT_INFO(...) ::Blazr::Log::getClientLogger()->info(__VA_ARGS__)
#define BLZR_CLIENT_WARN(...) ::Blazr::Log::getClientLogger()->warn(__VA_ARGS__)
#define BLZR_CLIENT_ERROR(...)                                                 \
  ::Blazr::Log::getClientLogger()->error(__VA_ARGS__)
#define BLZR_CLIENT_FATAL(...)                                                 \
  ::Blazr::Log::getClientLogger()->fatal(__VA_ARGS__)
#define BLZR_CLIENT_CRITICAL(...)                                              \
  ::Blazr::Log::getClientLogger()->critical(__VA_ARGS__)
