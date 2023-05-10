#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "Window.h"
#include "Device.h"
#include "Renderer.h"
#include "GameObject.h"
#include "systems/DefaultRenderSystem.h"
#include "systems/BillboardSystem.h"
#include "Camera.h"
#include "Descriptors.h"

namespace VulkanTutorial
{
	class App
	{
	public:
		App(std::string name="Window", int width=600, int height=600);
		~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		// [COMMENT] The main function of the application, calls Setup(), MainLoop() and Cleanup()
		void Run();

	protected:
		// [COMMENT] Everything that needs to be setup before the main loop starts
		virtual void Setup();
		// [COMMENT] Starts the main loop of the application, calls DrawFrame() every frame
		virtual void MainLoop();
		// [COMMENT] Called for each frame in the main loop
		virtual void DrawFrame();
		// [COMMENT] Called after the main loop ends
		virtual void Cleanup();

		// [COMMENT] The window object we display the application to
		Window window;
		// [COMMENT] The device object we use to interface with the GPU
		Device device;
		// [COMMENT] The renderer object we use to render the scene
		Renderer renderer;

		// [COMMENT] List of game objects in the scene
		std::unordered_map<unsigned int, GameObject> gameObjects;
	};
}