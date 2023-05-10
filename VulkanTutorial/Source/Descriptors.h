#pragma once

#include "Device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace VulkanTutorial
{
    // [COMMENT] A descriptor set layout is a description of the types of resources that will be bound to a descriptor set
    class DescriptorSetLayout
    {
    public:
        // [COMMENT] A builder class to help us build the descriptor set layout
        class Builder
        {
        public:
            Builder(Device& device) : device{ device } {}
            // [COMMENT] Creates a new binding at the "binding" location in  the bindings map
            Builder& AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);
            // [COMMENT] Builds the descriptor set layout from the bindings map
            std::unique_ptr<DescriptorSetLayout> Build() const;

        private:
            // [COMMENT] A reference to the device object we use to interface with the GPU (here it is used to create the descriptor set layout)
            Device& device;
            // [COMMENT] A map of bindings, where the key is the binding location
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~DescriptorSetLayout();
        DescriptorSetLayout(const DescriptorSetLayout&) = delete;
        DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

        // [COMMENT] Get the descriptor set layout object
        VkDescriptorSetLayout GetDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        Device& device;
        // [COMMENT] The descriptor set layout object combining all the bindings
        VkDescriptorSetLayout descriptorSetLayout;
        // [COMMENT] A map of bindings, where the key is the binding location (we get it from the builder)
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class DescriptorWriter;
    };

    // [COMMENT] A descriptor pool is a chunk of memory from which descriptor sets are allocated
    class DescriptorPool 
    {
    public:
        // [COMMENT] A builder class to help us build the descriptor pool
        class Builder 
        {
        public:
            Builder(Device& device) : device{ device } {}

            // [COMMENT] Adds a set size to the pool sizes vector
            Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
            // [COMMENT] Sets the pool flags
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
            // [COMMENT] Sets the maximum number of descriptor sets that can be allocated from the pool
            Builder& SetMaxSets(uint32_t count);
            // [COMMENT] Builds the descriptor pool
            std::unique_ptr<DescriptorPool> Build() const;

        private:
            // [COMMENT] A reference to the device object we use to interface with the GPU (here it is used to create the descriptor pool)
            Device& device;
            // [COMMENT] The number of sets and their sizes
            std::vector<VkDescriptorPoolSize> poolSizes{};
            // [COMMENT] The maximum number of descriptor sets that can be allocated from the pool
            uint32_t maxSets = 1000;
            // [COMMENT] Flags to specify certain supported operations on the pool
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        DescriptorPool(Device& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~DescriptorPool();
        DescriptorPool(const DescriptorPool&) = delete;
        DescriptorPool& operator=(const DescriptorPool&) = delete;

        // 
        bool AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        Device& device;
        // [COMMENT] The descriptor pool object
        VkDescriptorPool descriptorPool;

        friend class DescriptorWriter;
    };

    // [COMMENT] A descriptor writer is a helper class to write to a descriptor set
    class DescriptorWriter
    {
    public:
        DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);

        // [COMMENT] Writes buffer data to the descriptor set of binding index "binding"
        DescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        // [COMMENT] Writes image data to the descriptor set of binding index "binding"
        DescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);
        // [COMMENT] [TODO] Might want a WriteTexelBuffer function here using the pTexelBufferView member

        // [COMMENT] Creates a descriptor set or overwrites it if it already exists
        bool Build(VkDescriptorSet& set);
        // [COMMENT] Overwrites a descriptor set
        void Overwrite(VkDescriptorSet& set);

    private:
        // [COMMENT] A reference to the descriptor set layout object feeded to the constructor
        DescriptorSetLayout& setLayout;
        // [COMMENT] A reference to the descriptor pool object feeded to the constructor
        DescriptorPool& pool;
        // [COMMENT] A vector of write operations that are used to write to the descriptor set
        std::vector<VkWriteDescriptorSet> writes;
    };
}