#pragma once

#include "MyFirstEngine/Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace MyFirstEngine
{
	class VulkanWindow : public Window
	{
	public:
		VulkanWindow(const WindowInfo& info);
		virtual ~VulkanWindow();

		void OnUpdate() override;

		void SetEventCallback(const EventCallbackFn& callback) override { data.eventCallback = callback; }

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		VkExtent2D GetExtent() const { return { data.width, data.height }; }
		unsigned int GetWidth() const override { return data.width; }
		unsigned int GetHeight() const override { return data.height; }

		bool IsOpen() const;
		bool WasResized() { return framebufferResized; };
		void ResetResized() { framebufferResized = false; };

		GLFWwindow* GetGLFWindow() const { return window; };

	private:
		virtual void Init(const WindowInfo& info);
		virtual void Shutdown();
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

		GLFWwindow* window;

		struct WindowData
		{
			std::string title;
			unsigned int width;
			unsigned int height;
			EventCallbackFn eventCallback;
		};
		bool framebufferResized = false;

		WindowData data;
	};
}