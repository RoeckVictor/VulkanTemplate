#include "Mfepch.h"
#include "Shader.h"

#include <shaderc/shaderc.h>

#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace MyFirstEngine
{
	Shader::Shader(std::vector<char> vertCode, std::vector<char> fragCode)
		: m_VertCode(vertCode),
		  m_FragCode(fragCode)
	{
	}

	std::shared_ptr<Shader> Shader::CreateShaderFromFiles(const std::string& vertFilepath, const std::string& fragFilepath)
	{
		// shaderc::Compiler compiler;
		return std::make_shared<Shader>(std::vector<char>(), std::vector<char>());
	}

	std::shared_ptr<Shader> Shader::CreateShaderFromCompiledFiles(const std::string& vertFilepath, const std::string& fragFilepath)
	{

		return std::make_shared<Shader>(GetFileData(vertFilepath), GetFileData(fragFilepath));
	}

	std::vector<char> Shader::GetFileData(const std::string& path)
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