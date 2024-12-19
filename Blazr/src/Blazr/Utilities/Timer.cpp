#include "Blazr/Core/Log.h"
#include "Timer.h"
#include "sol.hpp"
#include <chrono>
#include <cstdint>

Blazr::Timer::Timer()
	: m_isRunning(false), m_isPaused(false), accumulatedTime(0),
	  m_stopThread(false), m_threadRunning(false) {}

Blazr::Timer::~Timer() {
	if (m_workerThread.joinable()) {
		m_stopThread = true;
		m_workerThread.join();
	}
}
void Blazr::Timer::Start() {

	if (!m_isRunning) {
		m_StartPoint = steady_clock::now();
		BLZR_CORE_INFO("Timer started!");
		m_isRunning = true;
		m_isPaused = false;
	}
}

void Blazr::Timer::Stop() {
	if (m_isRunning) {
		m_isRunning = false;
	}
	if (m_threadRunning) {
		if (m_workerThread.joinable()) {
			m_workerThread.join();
			m_stopThread = true;
		}
	}
}

void Blazr::Timer::Pause() {
	if (m_isRunning && !m_isPaused) {
		m_isPaused = true;
		m_PausedPoint = steady_clock::now();
	}
}

void Blazr::Timer::Resume() {
	if (m_isRunning && m_isPaused) {
		m_isPaused = false;
		m_StartPoint +=
			duration_cast<milliseconds>(steady_clock::now() - m_PausedPoint);
	}
}

const int64_t Blazr::Timer::ElapsedMS() {
	if (m_isRunning) {
		if (m_isPaused) {
			return duration_cast<milliseconds>(m_PausedPoint - m_StartPoint)
				.count();
		} else {
			return duration_cast<milliseconds>(steady_clock::now() -
											   m_StartPoint)
				.count();
		}
	}
	return 0;
}
void Blazr::Timer::ExecuteEvery(int64_t interval, sol::function luaFunc) {

	int64_t elapsedTime = ElapsedMS();

	accumulatedTime += elapsedTime;

	if (accumulatedTime >= interval) {
		luaFunc();
		accumulatedTime -= interval;
	}
	m_StartPoint = steady_clock::now();
}

void Blazr::Timer::ExecuteEveryAsync(int64_t interval, sol::function luaFunc) {

	if (m_threadRunning) {
		BLZR_CORE_WARN("Timer thread is already running!");
		return;
	}
	BLZR_CORE_ERROR("function called");
	m_stopThread = false;
	m_threadRunning = true;

	m_workerThread = std::thread([this, interval, luaFunc]() {
		Start(); // Sets m_StartPoint
		int64_t accumulatedTimeThread = 0;

		while (!m_stopThread) {
			// Calculate elapsed time since the last Start or loop iteration
			int64_t elapsedTime = ElapsedMS();
			accumulatedTimeThread += elapsedTime;

			int64_t tt =
				duration_cast<milliseconds>(m_StartPoint.time_since_epoch())
					.count();
			BLZR_CORE_INFO("Thread update {0}", tt);
			if (accumulatedTimeThread >= interval) {
				try {
					luaFunc();
				} catch (const std::exception &e) {
					BLZR_CORE_ERROR("Lua execution error: {0}", e.what());
				}
				accumulatedTimeThread -= interval;
			}

			std::this_thread::sleep_for(milliseconds(1));

			// Reset the start point only after elapsed time is added to
			// accumulatedTime
			m_StartPoint = steady_clock::now();
		}
		m_threadRunning = false;
	});
}

const bool Blazr::Timer::IsPaused() { return m_isPaused; }

const bool Blazr::Timer::IsRunning() { return m_isRunning; }

const int64_t Blazr::Timer::ElapsedSeconds() { return ElapsedMS() / 1000; }

void Blazr::Timer::CreateLuaTimerBind(sol::state &lua) {
	lua.new_usertype<Timer>(
		"Timer", sol::call_constructor, sol::default_constructor, "start",
		&Timer::Start, "pause", &Timer::Pause, "resume", &Timer::Resume,
		"is_paused", &Timer::IsPaused, "is_running", &Timer::IsRunning,
		"elapsed_ms", &Timer::ElapsedMS, "elapsed_seconds",
		&Timer::ElapsedSeconds, "execute_every", &Timer::ExecuteEvery,
		"execute_every_async", &Timer::ExecuteEveryAsync, "restart",
		[](Timer &timer) {
			if (timer.IsRunning()) {
				timer.Stop();
			}
			timer.Start();
		});
}
