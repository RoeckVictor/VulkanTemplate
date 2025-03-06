#include "Mfepch.h"
#include "Buffer.h"

#include <cassert>
#include <cstring>

namespace MyFirstEngine 
{
    VkDeviceSize Buffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
    {
        if (minOffsetAlignment > 0)
        {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }

        return instanceSize;
    }

    Buffer::Buffer(Device& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags,
                   VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment)
        : m_Device{ device },
          m_InstanceSize{ instanceSize },
          m_InstanceCount{ instanceCount },
          m_UsageFlags{ usageFlags },
          m_MemoryPropertyFlags{ memoryPropertyFlags } 
    {
        m_AlignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
        m_BufferSize = m_AlignmentSize * instanceCount;
        m_Device.CreateBuffer(m_BufferSize, usageFlags, memoryPropertyFlags, m_Buffer, m_Memory);
    }

    Buffer::~Buffer() 
    {
        Unmap();
        vkDestroyBuffer(m_Device.GetLogicalDevice(), m_Buffer, nullptr);
        vkFreeMemory(m_Device.GetLogicalDevice(), m_Memory, nullptr);
    }

    VkResult Buffer::Map(VkDeviceSize size, VkDeviceSize offset)
    {
        MFE_CORE_ASSERT(m_Buffer && m_Memory, "Called Map on buffer before create");
        return vkMapMemory(m_Device.GetLogicalDevice(), m_Memory, offset, size, 0, &m_Mapped);
    }

    void Buffer::Unmap()
    {
        if (m_Mapped)
        {
            vkUnmapMemory(m_Device.GetLogicalDevice(), m_Memory);
            m_Mapped = nullptr;
        }
    }

    void Buffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
    {
        MFE_CORE_ASSERT(m_Mapped, "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE) 
        {
            memcpy(m_Mapped, data, m_BufferSize);
        }
        else
        {
            char* memOffset = (char*)m_Mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    VkResult Buffer::Flush(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_Memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(m_Device.GetLogicalDevice(), 1, &mappedRange);
    }

    VkResult Buffer::Invalidate(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_Memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(m_Device.GetLogicalDevice(), 1, &mappedRange);
    }

    VkDescriptorBufferInfo Buffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset)
    {
        return VkDescriptorBufferInfo{ m_Buffer, offset, size };
    }

	void Buffer::WriteToIndex(void* data, int index)
    {
        WriteToBuffer(data, m_InstanceSize, index * m_AlignmentSize);
    }

    VkResult Buffer::FlushIndex(int index) { return Flush(m_AlignmentSize, index * m_AlignmentSize); }

    VkDescriptorBufferInfo Buffer::DescriptorInfoForIndex(int index)
    {
        return DescriptorInfo(m_AlignmentSize, index * m_AlignmentSize);
    }

    VkResult Buffer::InvalidateIndex(int index)
    {
        return Invalidate(m_AlignmentSize, index * m_AlignmentSize);
    }
}
