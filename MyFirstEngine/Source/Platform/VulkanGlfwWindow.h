#pragma once

#include "MyFirstEngine/Window.h"
#include "MyFirstEngine/Renderer/GraphicsContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace MyFirstEngine
{
	class VulkanGlfwWindow : public Window
	{
	public:
		VulkanGlfwWindow(const WindowInfo& info);
		virtual ~VulkanGlfwWindow();

		void BeginUpdate() override;
		void EndUpdate() override;

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.eventCallback = callback; }

		bool IsOpen() const;
		bool WasResized() { return m_FramebufferResized; };
		void ResetResized() { m_FramebufferResized = false; };

		VkExtent2D GetExtent() const { return { m_Data.width, m_Data.height }; }
		unsigned int GetWidth() const override { return m_Data.width; }
		unsigned int GetHeight() const override { return m_Data.height; }
		void* GetNativeWindow() const { return m_Window; };
		GraphicsContext* GetGraphicsContext() const { return m_GraphicsContext; };

	private:
		struct WindowData
		{
			std::string title;
			unsigned int width;
			unsigned int height;
			EventCallbackFn eventCallback;
		};

		virtual void Init(const WindowInfo& info);
		virtual void Shutdown();
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

		WindowData m_Data;
		GLFWwindow* m_Window;
		GraphicsContext* m_GraphicsContext;
		bool m_FramebufferResized = false;
	};
}