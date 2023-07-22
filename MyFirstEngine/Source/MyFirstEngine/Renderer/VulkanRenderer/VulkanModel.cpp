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
	struct hash<std::vector<MyFirstEngine::VertexAttribute>>
	{
		size_t operator()(std::vector<MyFirstEngine::VertexAttribute> const& vertex) const
		{
			size_t seed = 0;
			MyFirstEngine::HashCombine(seed, vertex[0].GetData<glm::vec3>());
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

	std::unique_ptr<Model> VulkanModel::CreateModelFromFile(const std::string& filepath, const VertexLayout layout)
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());

		VulkanModelData data{VulkanVertexArray(layout)};
		data.LoadModel(filepath);

		return std::make_unique<VulkanModel>(graphicsContext->GetDevice(), data);
	}

	std::unique_ptr<Model> VulkanModel::CreateModelFromData(const VulkanVertexArray vertices, const std::vector<uint32_t>& indices)
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());

		VulkanModelData data{ vertices, indices };

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

	void VulkanModel::CreateVertexBuffer(const VulkanVertexArray vertices)
	{
		vertexCount = vertices.count;
		uint32_t vertexSize = vertices.layout.GetStride();
		VkDeviceSize bufferSize = vertexSize * vertexCount;

		Buffer stagingBuffer
		{
			device, vertexSize, vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.GetDataBuffer().data());

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

		vertices.data.clear();
		indices.clear();

		std::unordered_map<std::vector<VertexAttribute>, uint32_t> uniqueVertices{};

		int vertexAttribs[] = {-1, -1, -1, -1};
		for(int i=0; i < vertices.layout.GetElements().size(); i++)
		{
			// Can't switch on strings
			std::string elementName = vertices.layout.GetElements()[i].name;
			if(elementName == "Position")
				vertexAttribs[0] = i;
			else if(elementName == "Color")
				vertexAttribs[1] = i;
			else if(elementName == "Normal")
				vertexAttribs[2] = i;
			else if(elementName == "TexCoord")
				vertexAttribs[3] = i;

			MFE_ASSERT(vertexAttribs[0] >= 0, "Vertex layout has no Position attribute");
		}

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				std::vector<VertexAttribute> vertex{};
				vertex.resize(vertices.layout.GetElements().size());

				if (vertexAttribs[0] >= 0 && index.vertex_index >= 0)
				{
					glm::vec3* position = new glm::vec3 {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};
					vertex[vertexAttribs[0]] = VertexAttribute(position, sizeof(glm::vec3));

					if(vertexAttribs[1] >= 0)
					{
						glm::vec3* color = new glm::vec3 {
							attrib.colors[index.vertex_index + 0],
							attrib.colors[index.vertex_index + 1],
							attrib.colors[index.vertex_index + 2]
						};
						vertex[vertexAttribs[1]] = VertexAttribute(color, sizeof(glm::vec3));
					}
				}

				if (vertexAttribs[2] >= 0 && index.normal_index >= 0)
				{
					glm::vec3* normal = new glm::vec3 {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
					vertex[vertexAttribs[2]] = VertexAttribute(normal, sizeof(glm::vec3));
				}

				if (vertexAttribs[3] >= 0 && index.texcoord_index >= 0)
				{
					glm::vec2* uv = new glm::vec2 {
						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
					};
					vertex[vertexAttribs[3]] = VertexAttribute(uv, sizeof(glm::vec2));
				}

				if (uniqueVertices.find(vertex) == uniqueVertices.end())
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.count);
					vertices.data.push_back(vertex);
					vertices.count++;
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}
}