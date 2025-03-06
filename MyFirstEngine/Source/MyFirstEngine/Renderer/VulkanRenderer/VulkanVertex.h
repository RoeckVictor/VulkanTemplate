#pragma once

#include "MyFirstEngine/Renderer/Vertex.h"

namespace MyFirstEngine
{
	static VkFormat VertexDataTypeToVkFormat(VertexDataType type)
	{
		switch (type)
		{
		case VertexDataType::Float: return VK_FORMAT_R32_SFLOAT;
		case VertexDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
		case VertexDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
		case VertexDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case VertexDataType::Mat3: return VK_FORMAT_R32G32B32_SFLOAT;
		case VertexDataType::Mat4: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case VertexDataType::Int: return VK_FORMAT_R32_SINT;
		case VertexDataType::Int2: return VK_FORMAT_R32G32_SINT;
		case VertexDataType::Int3: return VK_FORMAT_R32G32B32_SINT;
		case VertexDataType::Int4: return VK_FORMAT_R32G32B32A32_SINT;
		case VertexDataType::Bool: return VK_FORMAT_R8_SINT;
		}
		MFE_CORE_ASSERT(false, "Unknown VertexDataType!");
		return VK_FORMAT_UNDEFINED;
	}

	struct VulkanVertexArray : public VertexArray
	{
		VulkanVertexArray(const VertexLayout& layout)
			: VertexArray(layout)
		{
		}

		std::vector<VkVertexInputBindingDescription> GetBindingDescriptions()
		{
			std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
			bindingDescriptions[0].binding = 0;
			bindingDescriptions[0].stride = layout.GetStride();
			bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescriptions;
		}

		std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

			uint32_t offset = 0;
			for (uint32_t i = 0; i < layout.GetElements().size(); i++)
			{
				attributeDescriptions.push_back({ i, 0, VertexDataTypeToVkFormat(layout.GetElements()[i].type), offset });
				offset += layout.GetElements()[i].size;
			}

			return attributeDescriptions;
		}

		std::vector<char> GetDataBuffer() const
		{
			std::vector<char> buffer;

			for (const auto& vertex : data)
			{
				for (const auto& attribute : vertex)
				{
					char* attrData = static_cast<char*>(attribute.GetDataPointer());
					for (size_t i = 0; i < attribute.GetDataSize(); ++i)
					{
						buffer.push_back(attrData[i]);
					}
				}
			}

			return buffer;
		}
	};
}