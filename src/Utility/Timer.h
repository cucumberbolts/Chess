#pragma once

#include <chrono>

class Timer {
public:
	Timer(const char* name) : m_Name(name) {
		m_Start = std::chrono::high_resolution_clock::now();
	}

	~Timer() {
		if (!m_Stopped)
			Stop();
	}

	void Stop() {
		auto stopTime = std::chrono::high_resolution_clock::now();

		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_Start).time_since_epoch().count();
		auto stop = std::chrono::time_point_cast<std::chrono::microseconds>(stopTime).time_since_epoch().count();

		auto duration = stop - start;
		std::cout << m_Name << " time: " << duration * 0.001 << " milliseconds\n";

		m_Stopped = true;
	}
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	const char* m_Name;
	bool m_Stopped = false;
};
