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
		VulkanMaterial(const std::shared_ptr<Shader> shader);
		~VulkanMaterial();

		void Bind() const;
		void Unbind() const;

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		void CreatePipeline(VertexArray vertexArray);

		static std::unique_ptr<Material> CreateMatFromShader(const std::shared_ptr<Shader> shader);

	private:
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