#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "GraphicsContext.h"

#include "Device.h"
#include "Renderer.h"
#include "Descriptors.h"
#include "Buffer.h"
#include "Texture.h"

namespace MyFirstEngine
{
	#define MAX_LIGHTS 100

	struct PointLight
	{
		glm::vec4 position{};
		glm::vec4 color{};
	};

	struct UniformBufferObject
	{
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::mat4 inverseView{ 1.0f };
		glm::vec4 ambientColor{ 1.0f, 1.0f, 1.0f, 0.02f };
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(VulkanGlfwWindow* window);
		virtual ~VulkanContext();

		virtual void Init() override;
		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		inline Device& GetDevice() { return device; }
		inline Renderer& GetRenderer() { return renderer; }
		inline DescriptorPool& GetGlobalPool() { return *globalPool; }
		inline VkDescriptorSet GetGlobalSet(uint32_t index) { return globalDescriptorSets[index]; }
		inline Buffer& GetUniformBuffer(uint32_t index) { return *uniformBuffers[index]; }
		inline std::vector<std::unique_ptr<DescriptorSetLayout>>& GetGlobalSetLayout() { return globalSetLayouts; }

	private:
		VulkanGlfwWindow* window;

		Device device;
		Renderer renderer;

		std::unique_ptr<DescriptorPool> globalPool{};
		std::vector<std::unique_ptr<DescriptorSetLayout>> globalSetLayouts;
		std::vector<VkDescriptorSet> globalDescriptorSets;
		std::vector<std::unique_ptr<Buffer>> uniformBuffers;
	};
}