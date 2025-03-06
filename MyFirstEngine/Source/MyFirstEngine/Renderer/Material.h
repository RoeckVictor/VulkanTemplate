#pragma once
#include "VulkanRenderer/VulkanTexture.h"

#include "Vertex.h"
#include "Shader.h"

namespace MyFirstEngine
{
	struct Uniform
	{
		std::string name;
		std::vector<uint8_t> data;
	};

	class Material
	{
	public:
		static std::unique_ptr<Material> CreateMatFromShader(const std::shared_ptr<Shader> shader);

		virtual void CreatePipeline(VertexArray vertexArray) = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		void AddUniform(const uint32_t id, const std::string& name, const std::vector<uint8_t>& data, const bool isPushConstant = false);
		void UpdateUniform(const uint32_t id, const std::vector<uint8_t>& data, const bool isPushConstant = false);
		void RemoveUniform(const uint32_t id, const bool isPushConstant = false);
		void AddTexture(const uint32_t id, const std::string& name, const std::shared_ptr<Texture>& texture) { m_Textures[id] = texture; }

	protected:
		std::shared_ptr<Shader> m_Shader;
		std::unordered_map<uint32_t, Uniform> m_Uniforms;
		std::unordered_map<uint32_t, Uniform> m_PushConstants;
		std::unordered_map<uint32_t, std::shared_ptr<Texture>> m_Textures;
	};
}