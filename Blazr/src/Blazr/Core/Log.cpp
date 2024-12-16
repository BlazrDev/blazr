#include "blzrpch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "spdlog/spdlog-inl.h"

namespace Blazr {
std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

std::shared_ptr<ImGuiLogSink> Log::imGuiSink = std::make_shared<ImGuiLogSink>();
void Log::init() {
	// Create sinks
	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	consoleSink->set_pattern("%^[%T] %n: %v%$");
	imGuiSink->set_pattern("[%T] %n: %v");

	// Drop existing loggers if they exist
	spdlog::drop("BLAZR");
	spdlog::drop("APP");

	// Core logger
	s_CoreLogger = std::make_shared<spdlog::logger>(
		"BLAZR", spdlog::sinks_init_list{consoleSink, imGuiSink});
	s_CoreLogger->set_level(spdlog::level::trace);
	s_CoreLogger->flush_on(spdlog::level::trace);
	spdlog::register_logger(s_CoreLogger);

	// Client logger
	s_ClientLogger = std::make_shared<spdlog::logger>(
		"APP", spdlog::sinks_init_list{consoleSink, imGuiSink});
	s_ClientLogger->set_level(spdlog::level::trace);
	s_ClientLogger->flush_on(spdlog::level::trace);
	spdlog::register_logger(s_ClientLogger);
}
} // namespace Blazr
