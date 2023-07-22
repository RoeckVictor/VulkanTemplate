#include "Mfepch.h"
#include "Application.h"

#include "Log.h"

namespace MyFirstEngine
{
	Application* Application::instance = nullptr;

	Application::Application()
		: window((VulkanGlfwWindow*)VulkanGlfwWindow::Create()),
		  imguiLayer(new ImGuiLayer())
	{
		MFE_CORE_ASSERT(!instance, "Application already exists!");
		instance = this;
		window->SetEventCallback(MFE_BIND_EVENT_FN(Application::OnEvent));

		PushOverlay(imguiLayer);
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (isRunning)
		{
			for (Layer* layer : layerStack)
				layer->OnUpdate();

			imguiLayer->Begin();
			for (Layer* layer : layerStack)
				layer->OnImGuiRender();
			imguiLayer->End();

			window->EndUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(MFE_BIND_EVENT_FN(Application::OnWindowClose));
		
		for (auto it = layerStack.end(); it != layerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.isHandeld)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		isRunning = false;
		return true;
	}
}