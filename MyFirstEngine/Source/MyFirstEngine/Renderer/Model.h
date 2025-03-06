#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <memory>

#include "Vertex.h"

namespace MyFirstEngine
{
	class Model
	{
	public:
		static std::unique_ptr<Model> CreateModelFromFile(const std::string& filepath, const VertexLayout layout);
		static std::unique_ptr<Model> CreateModelFromData(const VertexArray vertices, const std::vector<uint32_t>& indices);

		virtual void Bind() const = 0;
		virtual void Draw() const = 0;

	protected:
		uint32_t m_VertexCount;
		uint32_t m_IndexCount;
		bool m_HasIndexBuffer = false;
	};
}