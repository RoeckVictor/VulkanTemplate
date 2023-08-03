#pragma once

namespace MyFirstEngine
{
	class Timestep
	{
	public:
		Timestep()
		{
			lastTime = std::chrono::high_resolution_clock::now();
			float deltaTime = 0.0f;
		}

		void UpdateTime()
		{
			auto newTime = std::chrono::high_resolution_clock::now();
			deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - lastTime).count();
			lastTime = newTime;
		}

		operator float() const { return deltaTime; }
		float GetSeconds() const { return deltaTime; }
		float GetMilliseconds() const { return deltaTime * 1000.0f; }

	private:
		std::chrono::steady_clock::time_point lastTime;
		float deltaTime;
	};
}