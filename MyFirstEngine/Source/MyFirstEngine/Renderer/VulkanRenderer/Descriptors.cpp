#include "Mfepch.h"
#include "Descriptors.h"

#include <cassert>
#include <stdexcept>
#include <iostream>

namespace MyFirstEngine
{
    DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(
        uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count)
    {
        MFE_PROFILE_FUNCTION();
        MFE_CORE_ASSERT(m_Bindings.count(binding) == 0, "Binding already in use");

        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        layoutBinding.pImmutableSamplers = nullptr;
        m_Bindings[binding] = layoutBinding;

        return *this;
    }

    std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const
    {
        MFE_PROFILE_FUNCTION();
        return std::make_unique<DescriptorSetLayout>(m_Device, m_Bindings);
    }

    DescriptorSetLayout::DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : m_Device{ device },
          m_Bindings{ bindings },
          m_DescriptorSetLayout{}
    {
        MFE_PROFILE_FUNCTION();
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto& kv : bindings)
        {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        MFE_CORE_ASSERT(vkCreateDescriptorSetLayout(device.GetLogicalDevice(), &descriptorSetLayoutInfo, nullptr, &m_DescriptorSetLayout) == VK_SUCCESS,
                        "failed to create descriptor set layout!");
    }

    DescriptorSetLayout::~DescriptorSetLayout() 
    {
        MFE_PROFILE_FUNCTION();
        vkDestroyDescriptorSetLayout(m_Device.GetLogicalDevice(), m_DescriptorSetLayout, nullptr);
    }

    DescriptorPool::Builder& DescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptorType, uint32_t count)
    {
        MFE_PROFILE_FUNCTION();
        m_PoolSizes.push_back({ descriptorType, count });
        return *this;
    }

    DescriptorPool::Builder& DescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags) 
    {
        MFE_PROFILE_FUNCTION();
        m_PoolFlags = flags;
        return *this;
    }
    DescriptorPool::Builder& DescriptorPool::Builder::SetMaxSets(uint32_t count) 
    {
        MFE_PROFILE_FUNCTION();
        m_MaxSets = count;
        return *this;
    }

    std::unique_ptr<DescriptorPool> DescriptorPool::Builder::Build() const 
    {
        MFE_PROFILE_FUNCTION();
        return std::make_unique<DescriptorPool>(m_Device, m_MaxSets, m_PoolFlags, m_PoolSizes);
    }

    DescriptorPool::DescriptorPool(Device& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes)
        : m_Device{ device } 
    {
        MFE_PROFILE_FUNCTION();
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        MFE_CORE_ASSERT(vkCreateDescriptorPool(device.GetLogicalDevice(), &descriptorPoolInfo, nullptr, &m_DescriptorPool) == VK_SUCCESS,
                        "failed to create descriptor pool!");
    }

    DescriptorPool::~DescriptorPool()
    {
        MFE_PROFILE_FUNCTION();
        vkDestroyDescriptorPool(m_Device.GetLogicalDevice(), m_DescriptorPool, nullptr);
    }

    bool DescriptorPool::AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const 
    {
        MFE_PROFILE_FUNCTION();
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_DescriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds a new pool whenever an old pool fills up
        return vkAllocateDescriptorSets(m_Device.GetLogicalDevice(), &allocInfo, &descriptor) == VK_SUCCESS;
    }

    void DescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const 
    {
        MFE_PROFILE_FUNCTION();
        vkFreeDescriptorSets(m_Device.GetLogicalDevice(), m_DescriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
    }

    void DescriptorPool::ResetPool() 
    {
        MFE_PROFILE_FUNCTION();
        vkResetDescriptorPool(m_Device.GetLogicalDevice(), m_DescriptorPool, 0);
    }

    DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool)
        : m_SetLayout{ setLayout }, m_Pool{ pool } {}

    DescriptorWriter& DescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo) 
        {
        MFE_PROFILE_FUNCTION();
        MFE_CORE_ASSERT(m_SetLayout.m_Bindings.count(binding) == 1, "Layout does not contain specified binding");

        auto& bindingDescription = m_SetLayout.m_Bindings[binding];

        MFE_CORE_ASSERT(bindingDescription.descriptorCount == 1, "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        m_Writes.push_back(write);
        return *this;
    }

    DescriptorWriter& DescriptorWriter::WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo) 
    {
        MFE_PROFILE_FUNCTION();
        MFE_CORE_ASSERT(m_SetLayout.m_Bindings.count(binding) == 1, "Layout does not contain specified binding");

        auto& bindingDescription = m_SetLayout.m_Bindings[binding];

        MFE_CORE_ASSERT(bindingDescription.descriptorCount == 1, "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        m_Writes.push_back(write);
        return *this;
    }

    bool DescriptorWriter::Build(VkDescriptorSet& set) 
    {
        MFE_PROFILE_FUNCTION();
        bool success = m_Pool.AllocateDescriptor(m_SetLayout.GetDescriptorSetLayout(), set);
        if (success)
        {
            Overwrite(set);
        }
        
        return success;
    }

    void DescriptorWriter::Overwrite(VkDescriptorSet& set) 
    {
        MFE_PROFILE_FUNCTION();
        for (auto& write : m_Writes)
        {
            write.dstSet = set;
        }

        vkUpdateDescriptorSets(m_Pool.m_Device.GetLogicalDevice(), static_cast<uint32_t>(m_Writes.size()), m_Writes.data(), 0, nullptr);
    }
}