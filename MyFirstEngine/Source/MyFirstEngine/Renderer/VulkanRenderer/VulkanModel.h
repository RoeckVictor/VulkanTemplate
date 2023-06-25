#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "MyFirstEngine/Renderer/Model.h"

#include "Device.h"
#include "Buffer.h"
#include "Texture.h"
#include "Descriptors.h"

namespace MyFirstEngine 
{
	class VulkanModel : public Model
	{
	public:
		struct VulkanVertex : public Vertex
		{
			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions()
			{
				std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
				bindingDescriptions[0].binding = 0;
				bindingDescriptions[0].stride = sizeof(VulkanVertex);
				bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return bindingDescriptions;
			}

			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
			{
				std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

				attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VulkanVertex, position) });
				attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VulkanVertex, color) });
				attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VulkanVertex, normal) });
				attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VulkanVertex, uv) });

				return attributeDescriptions;
			}
		};

		struct VulkanModelData
		{
			std::vector<VulkanVertex> vertices{};
			std::vector<uint32_t> indices{};

			void LoadModel(const std::string& filepath);
		};

		VulkanModel(Device& device, const VulkanModelData& builder);
		~VulkanModel();

		VulkanModel(const VulkanModel&) = delete;
		VulkanModel& operator=(const VulkanModel&) = delete;

		static std::unique_ptr<Model> CreateModelFromFile(const std::string& filepath);
		static std::unique_ptr<Model> CreateModelFromData(const std::vector<Model::Vertex>& vertices, const std::vector<uint32_t>& indices);

		void Bind() const override;
		void Draw() const override;

	private:
		void CreateVertexBuffer(const std::vector<VulkanVertex>& vertices);
		void CreateIndexBuffer(const std::vector<uint32_t>& indices);

		Device& device;
		std::unique_ptr<Buffer> vertexBuffer;
		std::unique_ptr<Buffer> indexBuffer;
	};
}