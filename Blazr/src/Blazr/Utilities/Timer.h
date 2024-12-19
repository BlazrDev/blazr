#pragma once
#include "sol.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

using namespace std::chrono;
namespace Blazr {
class Timer {

  private:
	time_point<steady_clock> m_StartPoint;
	time_point<steady_clock> m_PausedPoint;
	bool m_isRunning;
	bool m_isPaused;
	int64_t accumulatedTime;
	std::thread m_workerThread;
	std::atomic<bool> m_threadRunning;

  public:
	Timer();
	~Timer();

	Timer(const Timer &) = delete;
	Timer &operator=(const Timer &) = delete;

	Timer(Timer &&) = delete;
	Timer &operator=(Timer &&) = delete;

	void Start();
	void Stop();
	void Pause();
	void Resume();

	void ExecuteEvery(int64_t interval, sol::function luaFunc);
	void ExecuteEveryAsync(int64_t interval, sol::function luaFunc);
	void ExeuteNTimesAsync(int n, int64_t interval, sol::function luaFunc);
	void StopExecuting();
	const bool IsRunning();
	const bool IsPaused();

	const int64_t ElapsedMS();
	const int64_t ElapsedSeconds();
	static void CreateLuaTimerBind(sol::state &lua);
};

} // namespace Blazr
