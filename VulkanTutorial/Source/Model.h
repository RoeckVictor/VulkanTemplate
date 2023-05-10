#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "Device.h"
#include "Buffer.h"
#include "Texture.h"

namespace VulkanTutorial 
{
	class Model
	{
	public:
		// [COMMENT] Vertex data for the model (stores every per vertex data), has useful functions
		struct Vertex 
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			// [COMMENT] Two structures are required to describe vertex input data to Vulkan
			// [COMMENT] Binding description: Spacing between data and whether the data is per-vertex or per-instance (instanced rendering)
			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions()
			{
				std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
				bindingDescriptions[0].binding = 0;
				bindingDescriptions[0].stride = sizeof(Vertex);
				bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return bindingDescriptions;
			}

			// [COMMENT] Attribute descriptions: Type of the attributes passed to the vertex shader, which binding to load them from and at which offset
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
			{
				std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

				// [COMMENT] Each attribute description takes as argument in order: location, binding, format, offset
				// [COMMENT] Position attribute (location = 0)
				attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
				// [COMMENT] Color attribute (location = 1)
				attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
				// [COMMENT] Normal attribute (location = 2)
				attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
				// [COMMENT] UV attribute (location = 3)
				attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

				return attributeDescriptions;
			}

			// [COMMENT] Overload == operator to compare vertices
			bool operator==(const Vertex& other) const
			{
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Data 
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void LoadModel(const std::string& filepath);
		};
	
		Model(Device& device, const Data& builder);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		static std::unique_ptr<Model> CreateModelFromFile(Device& device, const std::string& filepath, const std::string& texturepath = "");

		// [COMMENT] Binds the vertex and index buffers to the command buffer
		void Bind(VkCommandBuffer commandBuffer);
		// [COMMENT] Draws the model using the bound buffers
		void Draw(VkCommandBuffer commandBuffer);

	private:
		// [COMMENT] Creates a vbo from a vector of vertices
		void CreateVertexBuffer(const std::vector<Vertex>& vertices);
		// [COMMENT] Creates a ibo from a vector of indices
		void CreateIndexBuffer(const std::vector<uint32_t>& indices);

		Device& device;

		// [COMMENT] The vertex buffer containing the vertex data of the model
		std::unique_ptr<Buffer> vertexBuffer;
		// [COMMENT] The number of vertices in the model
		uint32_t vertexCount;

		// [COMMENT] If we should use an index buffer to render the model
		bool hasIndexBuffer = false;
		// [COMMENT] The index buffer containing the indices of the triangles
		std::unique_ptr<Buffer> indexBuffer;
		// [COMMENT] The number of triangles to render divided by 3
		uint32_t indexCount;
	};
}