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
		// [COMMENT] Initializes the GLFW library
		if (!glfwInit())
			return false;

		// [COMMENT] Tells GLFW to not create an OpenGL context (Since it was designed for OpenGL)
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// [COMMENT] Tells GLFW to allow the resizing of the window
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		// [COMMENT] Creates the actuall GLFWwindow* object
		// [COMMENT] 4th parameter is to choose which monitor to use (nullptr for primary monitor)
		// [COMMENT] 5th parameter is only relevant to OpenGL
		window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		// [COMMENT] Sets the callback function for when the window is resized
		glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);

		return true;
	}

	void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		Window* newWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		// [COMMENT] Sets the framebufferResized flag to true
		newWindow->framebufferResized = true;
		// [COMMENT] Updates the width and height of the window
		newWindow->width = width;
		newWindow->height = height;
	}

	void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		// [COMMENT] Creates the VkSurfaceKHR object
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create window surface");
	}

	VkExtent2D Window::GetExtent() const
	{
		return {width, height};
	}

	void Window::Shutdown()
	{
		// [COMMENT] Destroys the GLFWwindow* object
		glfwDestroyWindow(window);
		// [COMMENT] Terminates GLFW
		glfwTerminate();
	}

	bool Window::IsOpen() const
	{
		// [COMMENT] Checks if the window should close (like when the user presses the X button)
		return !glfwWindowShouldClose(window);
	}

	void Window::PollEvents()
	{
		// [COMMENT] Polls for events (like keyboard input, mouse movement, etc.)
		glfwPollEvents();
	}
}