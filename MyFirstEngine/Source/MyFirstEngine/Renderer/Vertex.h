#pragma once

#include <vector>
#include <string>

namespace MyFirstEngine
{
	enum class VertexDataType
	{
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool
	};

	struct VertexElement
	{
		std::string name;
		VertexDataType type;
		uint32_t size;
		uint32_t offset;

		VertexElement(const std::string& name, VertexDataType type, uint32_t size)
			: name(name),
			type(type),
			size(size),
			offset(0)
		{
		}
	};

	class VertexLayout
	{
	public:
		VertexLayout(const std::initializer_list<VertexElement>& elements)
			: elements(elements)
		{
			uint32_t offset = 0;
			stride = 0;
			for (VertexElement element : elements)
			{
				element.offset = offset;
				offset += element.size;
				stride += element.size;
			}
		}

	inline const std::vector<VertexElement>& GetElements() const { return elements; }
	inline const uint32_t GetStride() const { return stride; }
	private:
		std::vector<VertexElement> elements;
		uint32_t stride;
	};

	struct VertexArray
	{
		VertexLayout layout;
		uint32_t count;
		std::vector<std::vector<void*>> data;

		VertexArray(const VertexLayout& layout)
			: layout(layout),
			count(0)
		{
			for (uint32_t i = 0; i < layout.GetElements().size(); i++)
			{
				data.push_back(std::vector<void*>{});
			}
		}
	};
}