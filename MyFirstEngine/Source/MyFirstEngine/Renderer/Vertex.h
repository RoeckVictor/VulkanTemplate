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

		VertexElement(const std::string& name, VertexDataType type, uint32_t size)
			: name(name),
			type(type),
			size(size)
		{
		}
	};

	class VertexLayout
	{
	public:
		VertexLayout(const std::initializer_list<VertexElement>& elements)
			: elements(elements)
		{
			stride = 0;
			for (VertexElement element : elements)
			{
				stride += element.size;
			}
		}

	inline const std::vector<VertexElement>& GetElements() const { return elements; }
	inline const uint32_t GetStride() const { return stride; }
	private:
		std::vector<VertexElement> elements;
		uint32_t stride;
	};

	class VertexAttribute
	{
	public:
		VertexAttribute() : data(), dataSize(0){}

		VertexAttribute(void* data, size_t dataSize)
			: data(data),
			  dataSize(dataSize)
		{
		}

		template <typename T>
		T GetData() const { return *(T*)data; }

		void* GetDataPointer() const { return data; }

		size_t GetDataSize() const { return dataSize; }

		bool operator==(const VertexAttribute& other) const
		{
			if (dataSize != other.dataSize)
				return false;

			// Cast data pointers to the appropriate type
			char* dataPtr = static_cast<char*>(data);
			char* otherDataPtr = static_cast<char*>(other.data);

			// Compare the data byte by byte
			for (size_t i = 0; i < dataSize; ++i)
			{
				if (dataPtr[i] != otherDataPtr[i])
					return false;
			}

			return true;
		}

	private:
		size_t dataSize;
		void* data;
	};

	struct VertexArray
	{
		VertexLayout layout;
		uint32_t count;
		std::vector<std::vector<VertexAttribute>> data;

		VertexArray(const VertexLayout& layout)
			: layout(layout),
			count(0)
		{
			for (uint32_t i = 0; i < layout.GetElements().size(); i++)
			{
				data.push_back(std::vector<VertexAttribute>{});
			}
		}
	};
}