#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "Device.h"
#include "Buffer.h"
#include "Texture.h"
#include "Descriptors.h"

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

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions()
			{
				std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
				bindingDescriptions[0].binding = 0;
				bindingDescriptions[0].stride = sizeof(Vertex);
				bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return bindingDescriptions;
			}

			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
			{
				std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

				attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
				attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
				attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
				attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

				return attributeDescriptions;
			}

			bool operator==(const Vertex& other) const
			{
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Data 
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
			std::string texturepath;

			void LoadModel(const std::string& filepath, std::string texturepath);
		};
	
		Model(Device& device, const Data& builder);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		static std::unique_ptr<Model> CreateModelFromFile(Device& device, const std::string& filepath, const std::string& texturepath = "");

		void CreateTextureSet(DescriptorSetLayout& setLayout, DescriptorPool& pool);
		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

		VkDescriptorSet textureSet{ VK_NULL_HANDLE }; //texture defaulted to null
		Texture texture;

	private:
		void CreateVertexBuffer(const std::vector<Vertex>& vertices);
		void CreateIndexBuffer(const std::vector<uint32_t>& indices);

		Device& device;

		std::unique_ptr<Buffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<Buffer> indexBuffer;
		uint32_t indexCount;
	};
}