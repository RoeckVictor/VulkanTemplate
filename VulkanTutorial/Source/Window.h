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
		
		// [COMMENT] Creates a VkSurfaceKHR object from window, this surface is used to render to the window
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
		// [COMMENT] Callback function for when the window is resized
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

		// [COMMENT] The GLFWwindow* object used to dispaly the scene
		GLFWwindow* window;
		// [COMMENT] Window parameters
		unsigned int width;
		unsigned int height;	
		std::string title;
		// [COMMENT] Used to check if the window was resized
		bool framebufferResized = false;
	};
}