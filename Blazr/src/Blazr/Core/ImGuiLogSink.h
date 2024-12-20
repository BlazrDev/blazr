#pragma once

#include "spdlog/common.h"
#include "spdlog/sinks/base_sink.h"
#include <mutex>
#include <string>
#include <vector>

namespace Blazr {

class ImGuiLogSink : public spdlog::sinks::base_sink<std::mutex> {
  public:
	struct LogEntry {
		std::string message;
		spdlog::level::level_enum level;
	};

	void flush_() override {}

	const std::vector<LogEntry> &getEntries() const { return m_LogEntries; }

	void clear() { m_LogEntries.clear(); }

  protected:
	void sink_it_(const spdlog::details::log_msg &msg) override {
		spdlog::memory_buf_t formatted;
		spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg,
																 formatted);
		m_LogEntries.push_back({fmt::to_string(formatted), msg.level});
	}

  private:
	std::vector<LogEntry> m_LogEntries;
};

} // namespace Blazr
