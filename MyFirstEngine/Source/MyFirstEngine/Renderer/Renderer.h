#pragma once

namespace MyFirstEngine 
{
	enum class SelectedAPI
	{
		None = 0,
		Vulkan = 1
	};

	class Renderer
	{
	public:
		inline static SelectedAPI GetSelectedAPI() { return selectedAPI; }

	private:
		static SelectedAPI selectedAPI;
	};
}