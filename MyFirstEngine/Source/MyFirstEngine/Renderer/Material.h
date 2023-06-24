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

		void AddPushConstant(size_t size, void* data);
		void RemovePushConstant(size_t index);

		void AddTexture(const std::string& name, const std::shared_ptr<Texture>& texture) { textures[name] = texture; }
		void AddFloat(const std::string& name, float value) { floats[name] = value; }
		void AddVec3(const std::string& name, const glm::vec3& value) { vec3s[name] = value; }
		void AddVec4(const std::string& name, const glm::vec4& value) { vec4s[name] = value; }
		void AddMat4(const std::string& name, const glm::mat4& value) { mat4s[name] = value; }

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		void CreatePipeline();

		static std::unique_ptr<Material> CreateMatFromFile(Device& device, const std::string& vertPath, const std::string& fragPath);

	private:
		std::vector<char> Material::ReadShaderFile(const std::string& path);
		void CreateTexturesSet(DescriptorSetLayout& setLayout, DescriptorPool& pool);
		uint32_t GetPushConstantsSize() const;

		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;

		std::vector<PushConstant> pushConstants;
		std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
		VkDescriptorSet textureSet{ VK_NULL_HANDLE };

		std::unordered_map<std::string, float> floats;
		std::unordered_map<std::string, glm::vec3> vec3s;
		std::unordered_map<std::string, glm::vec4> vec4s;
		std::unordered_map<std::string, glm::mat4> mat4s;

		Device& device;
		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};
}