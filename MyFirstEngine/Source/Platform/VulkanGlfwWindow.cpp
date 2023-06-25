#include "Mfepch.h"
#include "VulkanGlfwWindow.h"
#include "MyFirstEngine/Log.h"

#include "MyFirstEngine/Events/ApplicationEvent.h"
#include "MyFirstEngine/Events/KeyEvent.h"
#include "MyFirstEngine/Events/MouseEvent.h"

#include "MyFirstEngine/Renderer/VulkanRenderer/VulkanContext.h"

namespace MyFirstEngine
{
	static bool isGLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		MFE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowInfo& info)
	{
		return new VulkanGlfwWindow(info);
	}

	VulkanGlfwWindow::VulkanGlfwWindow(const WindowInfo& info)
	{
		Init(info);
	}

	VulkanGlfwWindow::~VulkanGlfwWindow()
	{
		Shutdown();
	}

	void VulkanGlfwWindow::Init(const WindowInfo& info)
	{
		data.title = info.title;
		data.width = info.width;
		data.height = info.height;

		MFE_CORE_INFO("Creating window {0} ({1}, {2})", info.title, info.width, info.height);

		if (!isGLFWInitialized)
		{
			int success = glfwInit();
			MFE_CORE_ASSERT(success, "Could not initialize GLFW!");

			isGLFWInitialized = true;

			glfwSetErrorCallback(GLFWErrorCallback);
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = glfwCreateWindow((int)info.width, (int)info.height, info.title.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, &data);
		glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);

		graphicsContext = new VulkanContext(this);
		graphicsContext->Init();

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.eventCallback(event);
		});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.eventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.eventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int keycode) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.eventCallback(event);
		});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.eventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.eventCallback(event);
		});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseMovedEvent event((float)xPos, (float)yPos);
			data.eventCallback(event);
		});
	}

	void VulkanGlfwWindow::Shutdown()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void VulkanGlfwWindow::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		VulkanGlfwWindow* newWindow = reinterpret_cast<VulkanGlfwWindow*>(glfwGetWindowUserPointer(window));
		newWindow->framebufferResized = true;
		newWindow->data.width = width;
		newWindow->data.height = height;
	}

	void VulkanGlfwWindow::BeginUpdate()
	{
		glfwPollEvents();
		graphicsContext->BeginFrame();
	}

	void VulkanGlfwWindow::EndUpdate()
	{
		graphicsContext->EndFrame();
	}

	void VulkanGlfwWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		VkResult success = glfwCreateWindowSurface(instance, window, nullptr, surface);
		MFE_CORE_ASSERT(success == VK_SUCCESS, "Failed to create window surface");
	}

	bool VulkanGlfwWindow::IsOpen() const
	{
		return !glfwWindowShouldClose(window);
	}
}