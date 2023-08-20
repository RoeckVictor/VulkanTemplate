#pragma once

namespace MyFirstEngine
{
	class Shader 
	{
	public:
		static std::shared_ptr<Shader> CreateShaderFromFiles(const std::vector<std::string> shaderFiles);
		static std::shared_ptr<Shader> CreateShaderFromCompiledFiles(const std::vector<std::string> compiledShaderFiles);

		std::vector<char> GetShaderCode(uint32_t index) { return m_ShaderCodes[index]; };

	protected:
		static std::vector<char> GetFileData(const std::string path);

		std::vector<std::vector<char>> m_ShaderCodes;
	};
}