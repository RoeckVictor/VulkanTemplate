#include "Descriptors.h"

// std
#include <cassert>
#include <stdexcept>

namespace VulkanTutorial 
{
    // [COMMENT] DescriptorSetLayout::Builder implementation
    DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count) 
    {
        assert(bindings.count(binding) == 0 && "Binding already in use");
        // [COMMENT] Structure used to describe a descriptor set layout binding
        VkDescriptorSetLayoutBinding layoutBinding{};
        // [COMMENT] The binding number, same number to get the data in the shader: layout(binding = x)
        layoutBinding.binding = binding;
        // [COMMENT] The type of descriptor (uniform buffer, sampler, etc)
        layoutBinding.descriptorType = descriptorType;
        // [COMMENT] We can send an array uniform buffer to the shader, this is the size of the array
        layoutBinding.descriptorCount = count;
        // [COMMENT] The stage of the shader the uniform will be used in (vertex, fragment, both, etc)
        layoutBinding.stageFlags = stageFlags;
        // [COMMENT] [TODO] Used for image sampling related descriptors
        layoutBinding.pImmutableSamplers = nullptr;
        // [COMMENT] Add the layoutBinding to the array of bindings at the binding number index
        bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const 
    {
        return std::make_unique<DescriptorSetLayout>(device, bindings);
    }

    // [COMMENT] DescriptorSetLayout implementation
    DescriptorSetLayout::DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : device{ device }, 
          bindings{ bindings },
          descriptorSetLayout{}
    {
        // [COMMENT] To use the binding we need a vector, so we copy the bindings to a vector
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto& kv : bindings)
            setLayoutBindings.push_back(kv.second);

        // [COMMENT] Structure used to create a descriptor set layout
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        // [COMMENT] The number of bindings
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        // [COMMENT] The array of bindings
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        // [COMMENT] Create the descriptor set layout and check for errors
        if (vkCreateDescriptorSetLayout(device.device(), &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
            throw std::runtime_error("failed to create descriptor set layout!");
    }

    DescriptorSetLayout::~DescriptorSetLayout() 
    {
        // [COMMENT] Destroy the descriptor set layout
        vkDestroyDescriptorSetLayout(device.device(), descriptorSetLayout, nullptr);
    }

    // [COMMENT] DescriptorPool::Builder implementation
    DescriptorPool::Builder& DescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptorType, uint32_t count)
    {
        // [COMMENT] Adding a pool to the list of pools to be created
        poolSizes.push_back({ descriptorType, count });
        return *this;
    }

    DescriptorPool::Builder& DescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags) 
    {
        // [COMMENT] Flags to indicate certain behavior of the descriptor pool
        poolFlags = flags;
        return *this;
    }
    DescriptorPool::Builder& DescriptorPool::Builder::SetMaxSets(uint32_t count) 
    {
        // [COMMENT] The maximum number of descriptor sets that can be allocated from the pool
        maxSets = count;
        return *this;
    }

    std::unique_ptr<DescriptorPool> DescriptorPool::Builder::Build() const 
    {
        // [COMMENT] Create the descriptor pool
        return std::make_unique<DescriptorPool>(device, maxSets, poolFlags, poolSizes);
    }

    // [COMMENT] DescriptorPool implementation
    DescriptorPool::DescriptorPool(Device& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes)
        : device{ device } 
    {
        // [COMMENT] Structure used to create a descriptor pool
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        // [COMMENT] Get the number of pools to be created (initialized in the builder)
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        // [COMMENT] Get the array of pool sizes to be created (initialized in the builder)
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        // [COMMENT] The maximum number of descriptor sets that can be allocated from the pool (initialized in the builder)
        descriptorPoolInfo.maxSets = maxSets;
        // [COMMENT] Flags to indicate certain behavior of the descriptor pool (initialized in the builder)
        descriptorPoolInfo.flags = poolFlags;

        // [COMMENT] Create the descriptor pool and check for errors
        if (vkCreateDescriptorPool(device.device(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS) 
            throw std::runtime_error("failed to create descriptor pool!");
    }

    DescriptorPool::~DescriptorPool()
    {
        // [COMMENT] Destroy the descriptor pool
        vkDestroyDescriptorPool(device.device(), descriptorPool, nullptr);
    }

    bool DescriptorPool::AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const 
    {
        // [COMMENT] Structure used to allocate descriptor sets
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        // [COMMENT] The descriptor pool to allocate from
        allocInfo.descriptorPool = descriptorPool;
        // [COMMENT] The number of descriptor sets to allocate and their layout
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        // [COMMENT] Allocate the descriptor set and check for errors
        if (vkAllocateDescriptorSets(device.device(), &allocInfo, &descriptor) != VK_SUCCESS)
            return false;

        return true;
    }

    void DescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const 
    {
        // [COMMENT] Free the descriptor sets
        vkFreeDescriptorSets(device.device(), descriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
    }

    void DescriptorPool::resetPool() 
    {
        vkResetDescriptorPool(device.device(), descriptorPool, 0);
    }

    // [COMMENT] DescriptorWriter implementation
    DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool)
        : setLayout{ setLayout }, pool{ pool } {}

    DescriptorWriter& DescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo) 
    {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        // [COMMENT] The VkDescriptorSetLayoutBinding struct for the specified binding
        auto& bindingDescription = setLayout.bindings[binding];

        assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

        // [COMMENT] Structure used to update a descriptor set
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        // [COMMENT] The type of the descriptor (uniform buffer, storage buffer, etc) to update
        write.descriptorType = bindingDescription.descriptorType;
        // [COMMENT] The binding index of the descriptor set to update
        write.dstBinding = binding;
        // [COMMENT] The data to be bound to the descriptors
        write.pBufferInfo = bufferInfo;
        // [COMMENT] The number of descriptors to update
        write.descriptorCount = 1;

        // [COMMENT] Add the write to the list of writes to be performed
        writes.push_back(write);
        return *this;
    }

    DescriptorWriter& DescriptorWriter::WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo) 
    {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        // [COMMENT] The VkDescriptorSetLayoutBinding struct for the specified binding
        auto& bindingDescription = setLayout.bindings[binding];

        assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

        // [COMMENT] Structure used to update a descriptor set
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        // [COMMENT] The type of the descriptor (uniform buffer, storage buffer, etc) to update
        write.descriptorType = bindingDescription.descriptorType;
        // [COMMENT] The binding index of the descriptor set to update
        write.dstBinding = binding;
        // [COMMENT] The data to be bound to the descriptors
        write.pImageInfo = imageInfo;
        // [COMMENT] The number of descriptors to update
        write.descriptorCount = 1;

        // [COMMENT] Add the write to the list of writes to be performed
        writes.push_back(write);
        return *this;
    }

    bool DescriptorWriter::Build(VkDescriptorSet& set) 
    {
        // [COMMENT] Allocate the descriptor set to the pool
        bool success = pool.AllocateDescriptor(setLayout.GetDescriptorSetLayout(), set);
        if (!success)
            return false;

        // [COMMENT] If the allocation failed, this means the set already exists, so we update it instead
        Overwrite(set);
        return true;
    }

    void DescriptorWriter::Overwrite(VkDescriptorSet& set) 
    {
        for (auto& write : writes)
            write.dstSet = set;

        // [COMMENT] Update the descriptor set with the writes
        vkUpdateDescriptorSets(pool.device.device(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    }
}