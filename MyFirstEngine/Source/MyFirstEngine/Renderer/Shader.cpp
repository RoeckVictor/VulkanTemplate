#include "Mfepch.h"
#include "Shader.h"

#include "Renderer.h"

#include "VulkanRenderer\VulkanShader.h"

namespace MyFirstEngine
{
	std::shared_ptr<Shader> Shader::CreateShaderFromFiles(const std::vector<std::string> shaderFiles)
	{
		switch (Renderer::GetSelectedAPI())
		{
			case RendererAPI::SelectedAPI::None: return nullptr;
			case RendererAPI::SelectedAPI::Vulkan: return VulkanShader::CreateShaderFromFiles(shaderFiles);
		}

		MFE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	std::shared_ptr<Shader> Shader::CreateShaderFromCompiledFiles(const std::vector<std::string> compiledShaderFiles)
	{
		switch (Renderer::GetSelectedAPI())
		{
			case RendererAPI::SelectedAPI::None: return nullptr;
			case RendererAPI::SelectedAPI::Vulkan: return VulkanShader::CreateShaderFromCompiledFiles(compiledShaderFiles);
		}

		MFE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	std::vector<char> Shader::GetFileData(const std::string path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		MFE_CORE_ASSERT(file.is_open(), "Failed to open file: " + path);

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}
}