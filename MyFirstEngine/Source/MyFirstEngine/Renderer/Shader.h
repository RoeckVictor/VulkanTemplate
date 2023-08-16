#pragma once

namespace MyFirstEngine
{
	class Shader 
	{
	public:
		Shader(std::vector<char> vertCode, std::vector<char> fragCode);

		static std::shared_ptr<Shader> CreateShaderFromFiles(const std::string& vertFilepath, const std::string& fragFilepath);
		static std::shared_ptr<Shader> CreateShaderFromCompiledFiles(const std::string& vertFilepath, const std::string& fragFilepath);

		std::vector<char> GetVertCode() { return m_VertCode; };
		std::vector<char> GetFragCode() { return m_FragCode; };

	private:
		static std::vector<char> GetFileData(const std::string& path);

		std::vector<char> m_VertCode;
		std::vector<char> m_FragCode;
	};
}