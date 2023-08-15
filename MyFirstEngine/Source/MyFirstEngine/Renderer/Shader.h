#pragma once

namespace MyFirstEngine
{
	class Shader 
	{
	public:
		Shader(std::vector<char> vertCode, std::vector<char> fragCode);

		static std::shared_ptr<Shader> CreateShaderFromFiles(const std::string& vertFilepath, const std::string& fragFilepath);
		static std::shared_ptr<Shader> CreateShaderFromCompiledFiles(const std::string& vertFilepath, const std::string& fragFilepath);

		std::vector<char> GetVertCode() { return vertCode; };
		std::vector<char> GetFragCode() { return fragCode; };

	private:
		static std::vector<char> GetFileData(const std::string& path);

		std::vector<char> vertCode;
		std::vector<char> fragCode;
	};
}