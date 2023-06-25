#include "Mfepch.h"
#include "VulkanModel.h"
#include "Utils.h"
#include "VulkanContext.h"
#include "MyFirstEngine/Application.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cstring>
#include <unordered_map>

namespace std 
{
	template<>
	struct hash<MyFirstEngine::VulkanModel::VulkanVertex>
	{
		size_t operator()(MyFirstEngine::VulkanModel::VulkanVertex const& vertex) const
		{
			size_t seed = 0;
			MyFirstEngine::HashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace MyFirstEngine
{
	VulkanModel::VulkanModel(Device& device, const VulkanModelData& builder)
		: device(device)
	{
		CreateVertexBuffer(builder.vertices);
		CreateIndexBuffer(builder.indices);
	}

	VulkanModel::~VulkanModel()
	{
	}

	std::unique_ptr<Model> VulkanModel::CreateModelFromFile(const std::string& filepath)
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());

		VulkanModelData data{};
		data.LoadModel(filepath);

		return std::make_unique<VulkanModel>(graphicsContext->GetDevice(), data);
	}

	std::unique_ptr<Model> VulkanModel::CreateModelFromData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());

		VulkanModelData data{};
		std::vector<VulkanVertex> vulkanVertices(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++)
		{
			vulkanVertices[i].position = vertices[i].position;
			vulkanVertices[i].color = vertices[i].color;
			vulkanVertices[i].normal = vertices[i].normal;
			vulkanVertices[i].uv = vertices[i].uv;
		}
		data.vertices = vulkanVertices;
		data.indices = indices;

		return std::make_unique<VulkanModel>(graphicsContext->GetDevice(), data);
	}

	void VulkanModel::Bind() const
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());
		VkCommandBuffer commandBuffer = graphicsContext->GetRenderer().GetCurrentCommandBuffer();

		VkBuffer vertexBuffers[] = { vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		if (hasIndexBuffer)
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanModel::Draw() const
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());
		VkCommandBuffer commandBuffer = graphicsContext->GetRenderer().GetCurrentCommandBuffer();

		if (hasIndexBuffer)
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		else
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}

	void VulkanModel::CreateVertexBuffer(const std::vector<VulkanVertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		uint32_t vertexSize = sizeof(vertices[0]);
		VkDeviceSize bufferSize = vertexSize * vertexCount;
		

		Buffer stagingBuffer
		{
			device, vertexSize, vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		vertexBuffer = std::make_unique<Buffer>(
			device, vertexSize, vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		device.CopyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}

	void VulkanModel::CreateIndexBuffer(const std::vector<uint32_t>& indices)
	{
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;

		if (!hasIndexBuffer)
			return;

		uint32_t indexSize = sizeof(indices[0]);
		VkDeviceSize bufferSize = indexSize * indexCount;

		Buffer stagingBuffer
		{
			device, indexSize, indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		indexBuffer = std::make_unique<Buffer>(
			device, indexSize, indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		device.CopyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}

	void VulkanModel::VulkanModelData::LoadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
			throw std::runtime_error(warn + err);

		vertices.clear();
		indices.clear();

		std::unordered_map<VulkanVertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				VulkanVertex vertex{};

				if (index.vertex_index >= 0)
				{
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.color = {
						attrib.colors[index.vertex_index + 0],
						attrib.colors[index.vertex_index + 1],
						attrib.colors[index.vertex_index + 2]
					};
				}

				if (index.normal_index >= 0)
				{
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (index.texcoord_index >= 0)
				{
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}
}