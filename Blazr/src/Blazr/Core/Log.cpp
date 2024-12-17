#include "blzrpch.h"
#include "Log.h"
#include "sol.hpp"
#include "spdlog/spdlog-inl.h"
#include <fmt/core.h>
#include <string>
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

void Log::log_trace(const std::string &format, sol::variadic_args args) {

	Log::getClientLogger()->trace(format_arguments(format, args).c_str());
}

void Log::log_info(const std::string &format, sol::variadic_args args) {
	Log::getClientLogger()->info(format_arguments(format, args).c_str());
}
void Log::log_warn(const std::string &format, sol::variadic_args args) {
	Log::getClientLogger()->warn(format_arguments(format, args).c_str());
}
void Log::log_error(const std::string &format, sol::variadic_args args) {
	Log::getClientLogger()->error(format_arguments(format, args).c_str());
}
void Log::log_critical(const std::string &format, sol::variadic_args args) {
	Log::getClientLogger()->critical(format_arguments(format, args).c_str());
}

void Log::createLogLuaBind(sol::state &lua) {
	lua.set_function("log_trace", &log_trace);
	lua.new_usertype<Log>(
		"Log", sol::no_constructor, "trace",
		[&](const std::string &format, sol::variadic_args args) {
			return log_trace(format, args);
		},
		"warn",
		[&](const std::string &format, sol::variadic_args args) {
			return log_warn(format, args);
		},
		"error",
		[&](const std::string &format, sol::variadic_args args) {
			return log_error(format, args);
		},

		"critical",
		[&](const std::string &format, sol::variadic_args args) {
			return log_critical(format, args);
		});
}

std::string Log::format_arguments(const std::string &format,
								  sol::variadic_args args) {
	std::vector<std::string> arg_strings;
	if (args.size() > 0) {

		for (auto arg : args) {
			// Convert all types to strings
			if (arg.is<std::string>()) {
				arg_strings.push_back(arg.as<std::string>());
			} else if (arg.is<int>()) {
				arg_strings.push_back(std::to_string(arg.as<int>()));
			} else if (arg.is<double>()) {
				arg_strings.push_back(std::to_string(arg.as<double>()));
			} else {
				arg_strings.push_back("<unknown>");
			}
		}
		std::string message = fmt::vformat(
			format, fmt::make_format_args(arg_strings[0], arg_strings[1],
										  arg_strings[2]));
		return message;
	} else {
		std::string message = format;
		return message;
	}

	// Use fmt::format with the collected arguments
}
} // namespace Blazr
