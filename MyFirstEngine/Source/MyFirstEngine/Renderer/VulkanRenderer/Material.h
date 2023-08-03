#pragma once

#include "Texture.h"
#include "Pipeline.h"

#include <string>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace MyFirstEngine
{
	struct PushConstant 
	{
		std::string name;
		size_t size;
		void* data;
	};

	class Material
	{
	public:
		Material(Device& device, const std::string& vertPath, const std::string& fragPath);
		~Material();
		// Material(const Material&) = delete;
		// Material& operator=(const Material&) = delete;

		void Bind() const;
		void Unbind() const;

		void AddPushConstant(const std::string& name, size_t size, void* data);
		void UpdatePushConstant(const std::string& name, void* data);
		void RemovePushConstant(const std::string& name);

		// TODO: Create dynamic DescriptorSets depending on the Unform values given to the Material
		void AddTexture(const std::string& name, const std::shared_ptr<Texture>& texture) { textures[name] = texture; }
		void AddFloat(const std::string& name, float value) { floats[name] = value; }
		void AddVec2(const std::string& name, const glm::vec2& value) { vec2s[name] = value; }
		void AddVec3(const std::string& name, const glm::vec3& value) { vec3s[name] = value; }
		void AddVec4(const std::string& name, const glm::vec4& value) { vec4s[name] = value; }
		void AddMat4(const std::string& name, const glm::mat4& value) { mat4s[name] = value; }

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		void CreatePipeline(std::vector<VkVertexInputBindingDescription> bindingDescriptions,
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions);

		static std::unique_ptr<Material> CreateMatFromFile(Device& device, const std::string& vertPath, const std::string& fragPath);

	private:
		std::vector<char> Material::ReadShaderFile(const std::string& path);
		void CreateTexturesSet();
		uint32_t GetPushConstantsSize() const;

		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;

		std::vector<PushConstant> pushConstants;
		std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
		VkDescriptorSet textureSet{ VK_NULL_HANDLE };

		std::unordered_map<std::string, float> floats;
		std::unordered_map<std::string, glm::vec2> vec2s;
		std::unordered_map<std::string, glm::vec3> vec3s;
		std::unordered_map<std::string, glm::vec4> vec4s;
		std::unordered_map<std::string, glm::mat4> mat4s;

		Device& device;
		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};
}