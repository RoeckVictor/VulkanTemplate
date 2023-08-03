#pragma once

#include "Core.h"

#include "Window.h"
#include "Platform/VulkanGlfwWindow.h"
#include "LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "MyFirstEngine/Engine/Timestep.h"

#include "ImGui/ImGuiLayer.h"

namespace MyFirstEngine
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline static Application& GetInstance() { return *instance; }
		inline VulkanGlfwWindow& GetWindow() { return *window; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		VulkanGlfwWindow* window;
		ImGuiLayer* imguiLayer;

		bool isRunning = true;
		LayerStack layerStack;
		Timestep timeStep;

		static Application* instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}