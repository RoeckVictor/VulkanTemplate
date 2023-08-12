#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "GraphicsContext.h"

#include "Renderer.h"
#include "Device.h"
#include "VulkanRenderer.h"
#include "Descriptors.h"
#include "Buffer.h"
#include "VulkanTexture.h"

namespace MyFirstEngine
{
	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(VulkanGlfwWindow* window);
		virtual ~VulkanContext();

		virtual void Init() override;
		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		inline Device& GetDevice() { return device; }
		inline VulkanRenderer& GetRenderer() { return renderer; }
		inline DescriptorPool& GetGlobalPool() { return *globalPool; }
		inline VkDescriptorSet GetGlobalSet(uint32_t index) { return globalDescriptorSets[index]; }
		inline Buffer& GetUniformBuffer(uint32_t index) { return *uniformBuffers[index]; }
		inline std::vector<std::unique_ptr<DescriptorSetLayout>>& GetGlobalSetLayout() { return globalSetLayouts; }

	private:
		VulkanGlfwWindow* window;

		Device device;
		VulkanRenderer renderer;

		std::unique_ptr<DescriptorPool> globalPool{};
		std::vector<std::unique_ptr<DescriptorSetLayout>> globalSetLayouts;
		std::vector<VkDescriptorSet> globalDescriptorSets;
		std::vector<std::unique_ptr<Buffer>> uniformBuffers;
	};
}