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

		inline Device& GetDevice() { return m_Device; }
		inline VulkanRenderer& GetRenderer() { return m_Renderer; }
		inline DescriptorPool& GetGlobalPool() { return *m_GlobalPool; }
		inline VkDescriptorSet GetGlobalSet(uint32_t index) { return m_GlobalDescriptorSets[index]; }
		inline Buffer& GetUniformBuffer(uint32_t index) { return *m_UniformBuffers[index]; }
		inline std::vector<std::unique_ptr<DescriptorSetLayout>>& GetGlobalSetLayout() { return m_GlobalSetLayouts; }

	private:
		VulkanGlfwWindow* m_Window;

		Device m_Device;
		VulkanRenderer m_Renderer;

		std::unique_ptr<DescriptorPool> m_GlobalPool{};
		std::vector<std::unique_ptr<DescriptorSetLayout>> m_GlobalSetLayouts;
		std::vector<VkDescriptorSet> m_GlobalDescriptorSets;
		std::vector<std::unique_ptr<Buffer>> m_UniformBuffers;
	};
}