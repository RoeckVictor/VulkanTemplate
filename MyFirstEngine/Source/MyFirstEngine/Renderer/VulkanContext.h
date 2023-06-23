#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "GraphicsContext.h"

#include "Device.h"
#include "Renderer.h"
#include "Descriptors.h"
#include "Buffer.h"
#include "Texture.h"
#include "systems/RenderSystem.h"

namespace MyFirstEngine
{
	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(VulkanGlfwWindow* window);
		virtual ~VulkanContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;

		inline Device& GetDevice() { return device; }
		inline Renderer& GetRenderer() { return renderer; }
		inline DescriptorPool& GetGlobalPool() { return *globalPool; }

	private:
		VulkanGlfwWindow* window;

		Device device;
		Renderer renderer;

		std::unique_ptr<DescriptorPool> globalPool{};
		std::vector<std::unique_ptr<DescriptorSetLayout>> globalSetLayouts;
	};
}