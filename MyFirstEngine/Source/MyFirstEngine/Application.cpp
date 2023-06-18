#include "Mfepch.h"
#include "Application.h"

#include "Log.h"

#include <GLFW/glfw3.h>

namespace MyFirstEngine
{
	Application* Application::instance = nullptr;

	Application::Application()
		: window((VulkanWindow*)VulkanWindow::Create()),
		  device(*window),
		  renderer(*window, device)
	{
		MFE_CORE_ASSERT(!instance, "Application already exists!");
		instance = this;
		window->SetEventCallback(MFE_BIND_EVENT_FN(Application::OnEvent));

		globalPool = DescriptorPool::Builder(device)
			.SetMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.SetPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
			.Build();

		globalSetLayouts.push_back(DescriptorSetLayout::Builder(device).Build());
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

			window->OnUpdate();
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