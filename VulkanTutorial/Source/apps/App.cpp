#include "apps/App.h"

#include <stdexcept>
#include <array>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Keyboard.h"
#include "Buffer.h"

namespace VulkanTutorial
{	
	App::App(std::string name, int width, int height)
		: window(name, width, height),
		  device(window),
		  renderer(window, device)
	{
	}

	App::~App()
	{
		
	}

	void App::Run()
	{
		Setup();
		MainLoop();
		Cleanup();
	}

	void App::Setup()
	{
	}

	void App::MainLoop()
	{
		// [COMMENT] Loop until we close the window
		while (window.IsOpen())
		{
			// [COMMENT] Poll for events from the window (e.g. keyboard or mouse input)
			window.PollEvents();

			// [COMMENT] Draw the current frame on the window
			DrawFrame();
		}
	}

	void App::DrawFrame()
	{
	}

	void App::Cleanup()
	{
		// [COMMENT] Wait for the logical device to finish before destroying resources
		vkDeviceWaitIdle(device.device());
	}
}