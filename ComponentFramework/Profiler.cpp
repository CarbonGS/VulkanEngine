#include "Profiler.h"

void Profiler::AddDuration(const std::string& label, std::chrono::duration<double, std::milli> duration)
{
	accumulated[label] += duration; // add duration if key exists, if not create new key
}

std::chrono::duration<double, std::milli> Profiler::GetElapsed(const std::string& label) const
{
	return accumulated.find(label)->second;
}

#include <iostream>

void Profiler::reset()
{
	for (auto& profile : accumulated)
	{
		profile.second = std::chrono::duration<double, std::milli>::zero();
	}
}

std::chrono::duration<double, std::milli> Profiler::GetElapsedTime(const std::string& label) const
{
	auto profile = accumulated.find(label);

	if (profile != accumulated.end())
	{
		return profile->second;
	}

	return std::chrono::duration<double, std::milli>::zero();
}

ScopedTimer::ScopedTimer(Profiler& profiler_, const std::string& label_) 
	: profiler(profiler_), label(label_), startTime(std::chrono::steady_clock::now())
{}

ScopedTimer::~ScopedTimer()
{
	std::chrono::duration<double, std::milli> elapsed = std::chrono::steady_clock::now() - startTime;
	profiler.AddDuration(label, elapsed);

}
