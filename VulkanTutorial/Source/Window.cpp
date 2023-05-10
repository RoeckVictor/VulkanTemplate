#include "Window.h"

namespace VulkanTutorial
{
	Window::Window(std::string title, unsigned int width, unsigned int height)
		: width(width), height(height), title(title)
	{
		if (!Initialize())
			throw std::runtime_error("Failed to initialize window");
	}

	Window::~Window()
	{
		Shutdown();
	}

	bool Window::Initialize()
	{
		if (!glfwInit())
			return false;

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);

		return true;
	}

	void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		Window* newWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		newWindow->framebufferResized = true;
		newWindow->width = width;
		newWindow->height = height;
	}

	void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create window surface");
	}

	VkExtent2D Window::GetExtent() const
	{
		return {width, height};
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	bool Window::IsOpen() const
	{
		return !glfwWindowShouldClose(window);
	}

	void Window::PollEvents()
	{
		glfwPollEvents();
	}
}