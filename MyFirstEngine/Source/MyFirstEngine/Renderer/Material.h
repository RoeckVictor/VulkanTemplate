#pragma once
#include "VulkanRenderer/Texture.h"

#include "Vertex.h"

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
		static std::unique_ptr<Material> CreateMatFromFile(const std::string& vertPath, const std::string& fragPath);

		virtual void CreatePipeline(VertexArray vertexArray) = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		void AddUniform(const uint32_t id, const std::string& name, const std::vector<uint8_t>& data, const bool isPushConstant = false);
		void UpdateUniform(const uint32_t id, const std::vector<uint8_t>& data, const bool isPushConstant = false);
		void RemoveUniform(const uint32_t id, const bool isPushConstant = false);

		void AddTexture(const uint32_t id, const std::string& name, const std::shared_ptr<Texture>& texture) { textures[id] = texture; }

	protected:
		std::unordered_map<uint32_t, Uniform> uniforms;
		std::unordered_map<uint32_t, Uniform> pushConstants;
		std::unordered_map<uint32_t, std::shared_ptr<Texture>> textures;
	};
}