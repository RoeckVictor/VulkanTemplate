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
			: m_Elements(elements)
		{
			m_Stride = 0;
			for (VertexElement element : elements)
			{
				m_Stride += element.size;
			}
		}

	inline const std::vector<VertexElement>& GetElements() const { return m_Elements; }
	inline const uint32_t GetStride() const { return m_Stride; }

	private:
		std::vector<VertexElement> m_Elements;
		uint32_t m_Stride;
	};

	class VertexAttribute
	{
	public:
		VertexAttribute() : m_Data(), m_DataSize(0){}

		VertexAttribute(void* data, size_t dataSize)
			: m_Data(data),
			  m_DataSize(dataSize)
		{
		}

		template <typename T>
		T GetData() const { return *(T*)m_Data; }

		void* GetDataPointer() const { return m_Data; }

		size_t GetDataSize() const { return m_DataSize; }

		bool operator==(const VertexAttribute& other) const
		{
			if (m_DataSize != other.m_DataSize) { return false; }

			// Cast data pointers to the appropriate type
			char* dataPtr = static_cast<char*>(m_Data);
			char* otherDataPtr = static_cast<char*>(other.m_Data);

			// Compare the data byte by byte
			for (size_t i = 0; i < m_DataSize; ++i)
			{
				if (dataPtr[i] != otherDataPtr[i])
					return false;
			}

			return true;
		}

	private:
		size_t m_DataSize;
		void* m_Data;
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