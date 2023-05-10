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

		void Run();

	protected:
		virtual void Setup();
		virtual void MainLoop();
		virtual void DrawFrame();
		virtual void Cleanup();

		Window window;
		Device device;
		Renderer renderer;

		std::unordered_map<unsigned int, GameObject> gameObjects;
	};
}