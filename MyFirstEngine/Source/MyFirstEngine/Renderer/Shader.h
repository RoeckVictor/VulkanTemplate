#pragma once

namespace MyFirstEngine
{
	struct ShaderProperty {
		enum class Type {
			Float,
			Float2,
			Float3,
			Float4,
			Mat3,
			Mat4,
			Int,
			Bool,
			Texture2D,
			TextureCube,
			Struct,
			Array
		};

		enum class Usage {
			Input,
			Output,
			Uniform,
			PushConstant,
			Sampler
		};

		struct StructMember {
			std::string name;
			Type type;
			std::vector<uint32_t> arraySizes;  // Empty means not an array
			std::vector<StructMember> members;  // For nested structs
		};

		std::string name;
		Type type;
		Usage usage;
		uint32_t stages; // Bit flags for shader stages
		uint32_t binding;
		uint32_t set;
		bool isPushConstant;
		std::vector<uint8_t> data;
		std::vector<uint32_t> arraySizes;  // Empty means not an array
		std::vector<StructMember> members;  // For struct types
	};

	class Shader 
	{
	public:
		static std::shared_ptr<Shader> CreateShaderFromFiles(const std::vector<std::string> shaderFiles);
		static std::shared_ptr<Shader> CreateShaderFromCompiledFiles(const std::vector<std::string> compiledShaderFiles);

		std::vector<char> GetShaderCode(uint32_t index) { return m_ShaderCodes[index]; };
		const std::unordered_map<std::string, ShaderProperty>& GetProperties() const { return m_Properties; }

	protected:
		static std::vector<char> GetFileData(const std::string path);

		std::vector<std::vector<char>> m_ShaderCodes;
		std::unordered_map<std::string, ShaderProperty> m_Properties;
	};
}