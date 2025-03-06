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

		void PrintShaderProperties() const;

	private:
		void CreateTexturesSet();
		uint32_t GetPushConstantsSize() const;

		VkDescriptorSet m_TextureSet{ VK_NULL_HANDLE };

		Device& m_Device;
		std::unique_ptr<Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}