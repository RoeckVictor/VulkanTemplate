#include "Mfepch.h"
#include "MyFirstEngine/Application.h"
#include "VulkanContext.h"

#include "VulkanShader.h"

#include <shaderc/shaderc.h>

namespace MyFirstEngine
{
	VulkanShader::VulkanShader(std::vector<std::vector<char>> shaderCodes)
		: m_Device(static_cast<MyFirstEngine::VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext())->GetDevice())
	{
		m_ShaderCodes = shaderCodes;
		for(std::vector<char> code : m_ShaderCodes)
		{
			SpvReflectShaderModule reflectModule;
			// MFE_ASSERT(spvReflectCreateShaderModule(code.size(), static_cast<void*>(&code), &reflectModule) == SPV_REFLECT_RESULT_SUCCESS,
			//	       "Failed to create shader reflect module");
			m_ReflectModules.push_back(reflectModule);

			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

			VkShaderModule shaderModule;
			MFE_CORE_ASSERT(vkCreateShaderModule(m_Device.GetLogicalDevice(), &createInfo, nullptr, &shaderModule) == VK_SUCCESS, "Failed to create shader module!");
			m_ShaderModules.push_back(shaderModule);
		}
	}

	VulkanShader::~VulkanShader()
	{
		for (VkShaderModule shaderModule : m_ShaderModules)
		{
			vkDestroyShaderModule(m_Device.GetLogicalDevice(), shaderModule, nullptr);
		}

		for (SpvReflectShaderModule reflectModule : m_ReflectModules)
		{
			spvReflectDestroyShaderModule(&reflectModule);
		}
	}

	std::shared_ptr<Shader> VulkanShader::CreateShaderFromFiles(const std::vector<std::string> shaderFiles)
	{
		// shaderc::Compiler compiler;
		return std::make_shared<VulkanShader>(std::vector<std::vector<char>>());
	}

	std::shared_ptr<Shader> VulkanShader::CreateShaderFromCompiledFiles(const std::vector<std::string> compiledShaderFiles)
	{
		std::vector<std::vector<char>> filesData;
		for (std::string filePath : compiledShaderFiles)
		{
			filesData.push_back(Shader::GetFileData(filePath));
		}
		
		return std::make_shared<VulkanShader>(filesData);
	}
}