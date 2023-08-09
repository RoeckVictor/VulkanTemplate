#pragma once
#include "VulkanRenderer/Texture.h"

#include "Vertex.h"

namespace MyFirstEngine
{
	struct Uniform
	{
		std::string name;
		size_t size;
		void* data;
	};

	class Material
	{
	public:
		static std::unique_ptr<Material> CreateMatFromFile(const std::string& vertPath, const std::string& fragPath);

		virtual void CreatePipeline(VertexArray vertexArray) = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		void AddUniform(const std::string& name, size_t size, void* data, const bool isPushConstant = false);
		void RemoveUniform(const std::string& name, const bool isPushConstant = false);

		void AddTexture(const std::string& name, const std::shared_ptr<Texture>& texture) { textures[name] = texture; }

	protected:
		std::unordered_map<std::string, Uniform> uniforms;
		std::unordered_map<std::string, Uniform> pushConstants;
		std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
	};
}