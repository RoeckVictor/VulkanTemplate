#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <stdexcept>

namespace VulkanTutorial
{
	class Window
	{
	public:
		Window(std::string title, unsigned int width, unsigned int height);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		VkExtent2D GetExtent() const;
		void Shutdown();
		bool IsOpen() const;
		bool WasResized() { return framebufferResized; };
		void ResetResized() { framebufferResized = false; };
		void PollEvents();

		GLFWwindow* GetWindow() const { return window; };

	private:
		bool Initialize();
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

		GLFWwindow* window;
		unsigned int width;
		unsigned int height;	
		std::string title;
		bool framebufferResized = false;
	};
}