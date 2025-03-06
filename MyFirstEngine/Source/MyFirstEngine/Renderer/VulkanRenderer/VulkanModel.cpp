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
		: m_Device(device)
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

	std::unique_ptr<Model> VulkanModel::CreateModelFromData(const VertexArray vertices, const std::vector<uint32_t>& indices)
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());

		VulkanVertexArray vulkanVertices(vertices.layout);
		for (const auto& vertex : vertices.data)
		{
			std::vector<VertexAttribute> vulkanVertex;

			for (const auto& attribute : vertex)
			{
				VertexAttribute vulkanAttribute(attribute.GetDataPointer(), attribute.GetDataSize());
				vulkanVertex.push_back(vulkanAttribute);
			}

			vulkanVertices.data.push_back(vulkanVertex);
		}
		vulkanVertices.count = vertices.count;

		VulkanModelData data{vulkanVertices, indices};

		return std::make_unique<VulkanModel>(graphicsContext->GetDevice(), data);
	}

	void VulkanModel::Bind() const
	{
		if (m_VertexBuffer == nullptr) { return; }

		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());
		VkCommandBuffer commandBuffer = graphicsContext->GetRenderer().GetCurrentCommandBuffer();

		VkBuffer vertexBuffers[] = { m_VertexBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		if (m_HasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void VulkanModel::Draw() const
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());
		VkCommandBuffer commandBuffer = graphicsContext->GetRenderer().GetCurrentCommandBuffer();

		if (m_HasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
		}
		else 
		{
			vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
		}
	}

	void VulkanModel::CreateVertexBuffer(const VulkanVertexArray vertices)
	{
		m_VertexCount = vertices.count;
		uint32_t vertexSize = vertices.layout.GetStride();
		VkDeviceSize bufferSize = vertexSize * m_VertexCount;

		if (bufferSize == 0) { return; }

		Buffer stagingBuffer
		{
			m_Device, vertexSize, m_VertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)vertices.GetDataBuffer().data());

		m_VertexBuffer = std::make_unique<Buffer>(
			m_Device, vertexSize, m_VertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_Device.CopyBuffer(stagingBuffer.GetBuffer(), m_VertexBuffer->GetBuffer(), bufferSize);
	}

	void VulkanModel::CreateIndexBuffer(const std::vector<uint32_t>& indices)
	{
		m_IndexCount = static_cast<uint32_t>(indices.size());
		m_HasIndexBuffer = m_IndexCount > 0;

		if (!m_HasIndexBuffer) { return; }

		uint32_t indexSize = sizeof(indices[0]);
		VkDeviceSize bufferSize = indexSize * m_IndexCount;

		Buffer stagingBuffer
		{
			m_Device, indexSize, m_IndexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)indices.data());

		m_IndexBuffer = std::make_unique<Buffer>(
			m_Device, indexSize, m_IndexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_Device.CopyBuffer(stagingBuffer.GetBuffer(), m_IndexBuffer->GetBuffer(), bufferSize);
	}

	void VulkanModel::VulkanModelData::LoadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		MFE_CORE_ASSERT(tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()), warn + err);

		vertices.data.clear();
		indices.clear();

		std::unordered_map<std::vector<VertexAttribute>, uint32_t> uniqueVertices{};

		int vertexAttribs[] = {-1, -1, -1, -1};
		for(int i=0; i < vertices.layout.GetElements().size(); i++)
		{
			// Can't switch on strings
			std::string elementName = vertices.layout.GetElements()[i].name;
			if(elementName == "Position") { vertexAttribs[0] = i; }
			else if(elementName == "Color") { vertexAttribs[1] = i; }
			else if(elementName == "Normal") { vertexAttribs[2] = i; }	
			else if(elementName == "TexCoord") { vertexAttribs[3] = i; }
		}
		MFE_ASSERT(vertexAttribs[0] >= 0, "Vertex layout has no Position attribute");

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