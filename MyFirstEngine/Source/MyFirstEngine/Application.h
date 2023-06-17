#pragma once

#include "Core.h"

#include "Window.h"
#include "Platform/VulkanWindow.h"
#include "LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Renderer/Device.h"
#include "Renderer/Renderer.h"
#include "Renderer/Descriptors.h"
#include "Renderer/Buffer.h"
#include "Renderer/Texture.h"
#include "Renderer/systems/RenderSystem.h"

namespace MyFirstEngine
{
	class MFE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline static Application& GetInstance() { return *instance; }
		inline VulkanWindow& GetWindow() { return *window; }
		inline Device& GetDevice() { return device; }
		inline Renderer& GetRenderer() { return renderer; }
		inline DescriptorPool& GetGlobalPool() { return *globalPool; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		VulkanWindow* window;
		Device device;
		Renderer renderer;

		bool isRunning = true;
		LayerStack layerStack;

		std::unique_ptr<DescriptorPool> globalPool{};
		std::vector<std::unique_ptr<DescriptorSetLayout>> globalSetLayouts;

		static Application* instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}