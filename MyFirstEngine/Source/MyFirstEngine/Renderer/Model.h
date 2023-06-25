#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <memory>

namespace MyFirstEngine
{
	class Model
	{
	public:
		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			bool operator==(const Vertex& other) const
			{
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		static std::unique_ptr<Model> CreateModelFromFile(const std::string& filepath);
		static std::unique_ptr<Model> CreateModelFromData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		virtual void Bind() const = 0;
		virtual void Draw() const = 0;

	protected:
		uint32_t vertexCount;
		uint32_t indexCount;
		bool hasIndexBuffer = false;
	};
}