#include "Mfepch.h"
#include "Application.h"

#include "Log.h"

namespace MyFirstEngine
{
	Application* Application::m_Instance = nullptr;

	Application::Application()
		: m_Window((VulkanGlfwWindow*)VulkanGlfwWindow::Create()),
		  m_ImguiLayer(new ImGuiLayer())
	{
		MFE_CORE_ASSERT(!m_Instance, "Application already exists!");
		m_Instance = this;
		m_Window->SetEventCallback(MFE_BIND_EVENT_FN(Application::OnEvent));

		PushOverlay(m_ImguiLayer);
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_IsRunning)
		{
			m_TimeStep.UpdateTime();

			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate(m_TimeStep);
			}
				
			m_ImguiLayer->Begin();
			for (Layer* layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}	
			m_ImguiLayer->End();

			m_Window->EndUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(MFE_BIND_EVENT_FN(Application::OnWindowClose));
		
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.isHandeld) { break; }
				
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_IsRunning = false;
		return true;
	}
}