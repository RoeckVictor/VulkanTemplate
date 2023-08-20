#pragma once

#include <vulkan/vulkan.h>
#include <spirv_reflect.h>

#include "Shader.h"

namespace MyFirstEngine 
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(std::vector<std::vector<char>> shaderCodes);
		~VulkanShader();

		static std::shared_ptr<Shader> CreateShaderFromFiles(const std::vector<std::string> shaderFiles);
		static std::shared_ptr<Shader> CreateShaderFromCompiledFiles(const std::vector<std::string> compiledShaderFiles);

		SpvReflectShaderModule GetReflectModule(uint32_t index) { return m_ReflectModules[index]; };
		VkShaderModule GetShaderModule(uint32_t index) { return m_ShaderModules[index]; };

	private:
		Device& m_Device;

		std::vector<SpvReflectShaderModule> m_ReflectModules;
		std::vector<VkShaderModule> m_ShaderModules;
	};
}