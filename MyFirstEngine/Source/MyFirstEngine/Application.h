#pragma once

#include "MyFirstEngine/Core.h"

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

		inline static Application& GetInstance() { return *m_Instance; }
		inline VulkanGlfwWindow& GetWindow() { return *m_Window; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		VulkanGlfwWindow* m_Window;
		ImGuiLayer* m_ImguiLayer;

		bool m_IsRunning = true;
		LayerStack m_LayerStack;
		Timestep m_TimeStep;

		static Application* m_Instance;
	};

	Application* CreateApplication();
}