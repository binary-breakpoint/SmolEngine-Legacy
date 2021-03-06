#pragma once
#include <chrono>

namespace SmolEngine
{
	class Profiler
	{
	public:
		void    Start();
		void    Finish();
		size_t  GetTimeInMilliseconds();

	private:
		std::chrono::high_resolution_clock::time_point m_Start{};
		size_t m_Time = 0;
	};
}