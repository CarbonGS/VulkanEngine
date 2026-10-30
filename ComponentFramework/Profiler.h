#pragma once
#include <chrono>
#include <unordered_map>

class Profiler {

private:
	std::unordered_map<std::string, std::chrono::steady_clock::time_point> startTime; // map of multiple profilers
	std::unordered_map< std::string, std::chrono::duration<double, std::milli>> accumulated; // map of total execution time per profiler

public:
	Profiler() = default;

	// Called by scoped timer destructor to write down specific profiles stats to our accumulated map 
	void AddDuration(const std::string& label, std::chrono::duration<double, std::milli> duration);

	// Returns time elapsed for specific profiler
	std::chrono::duration<double, std::milli> GetElapsed(const std::string& label) const;

	// Resets every profile accumulated to 0
	void reset(); 

	std::chrono::duration<double, std::milli> GetElapsedTime(const std::string& label) const;	
};

class ScopedTimer
{
private:
	Profiler& profiler;
	std::string label;
	std::chrono::steady_clock::time_point startTime;

public:
	ScopedTimer(Profiler& profiler_, const std::string& label_);
	~ScopedTimer();
};