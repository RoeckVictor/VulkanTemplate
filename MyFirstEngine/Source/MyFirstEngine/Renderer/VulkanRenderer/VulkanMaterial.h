#pragma once
#include "Material.h"
#include "Pipeline.h"

#include <string>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace MyFirstEngine
{
	class VulkanMaterial : public Material
	{
	public:
		VulkanMaterial(Device& device, const std::string& vertPath, const std::string& fragPath);
		~VulkanMaterial();
		// VulkanMaterial(const VulkanMaterial&) = delete;
		// VulkanMaterial& operator=(const VulkanMaterial&) = delete;

		void Bind() const;
		void Unbind() const;

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		void CreatePipeline(VertexArray vertexArray);

		static std::unique_ptr<Material> CreateMatFromFile(Device& device, const std::string& vertPath, const std::string& fragPath);

	private:
		std::vector<char> VulkanMaterial::ReadShaderFile(const std::string& path);
		void CreateTexturesSet();
		uint32_t GetPushConstantsSize() const;

		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;

		VkDescriptorSet textureSet{ VK_NULL_HANDLE };

		Device& device;
		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};
}