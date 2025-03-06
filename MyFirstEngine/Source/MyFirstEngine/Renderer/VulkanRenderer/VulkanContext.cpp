#include "Mfepch.h"
#include "VulkanContext.h"

namespace MyFirstEngine
{
	VulkanContext::VulkanContext(VulkanGlfwWindow* window)
		: m_Window(window),
		  m_Device(*window),
		  m_Renderer(*window, m_Device)
	{
		
	}

	VulkanContext::~VulkanContext()
	{

	}

	void VulkanContext::Init()
	{
		m_UniformBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < m_UniformBuffers.size(); i++)
		{
			m_UniformBuffers[i] = std::make_unique<Buffer>(
				m_Device,
				sizeof(Renderer::SceneUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			m_UniformBuffers[i]->Map();
		}

		uint32_t maxObjectsPerFrame = 4;
		uint32_t maxSets = SwapChain::MAX_FRAMES_IN_FLIGHT * maxObjectsPerFrame;

		m_GlobalPool = DescriptorPool::Builder(m_Device)
			.SetMaxSets(maxSets + SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxSets)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxSets + SwapChain::MAX_FRAMES_IN_FLIGHT)
			.SetPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
			.Build();

		m_GlobalSetLayouts.push_back(DescriptorSetLayout::Builder(m_Device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.Build());

		m_GlobalSetLayouts.push_back(DescriptorSetLayout::Builder(m_Device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
			.Build());

		m_GlobalDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < m_GlobalDescriptorSets.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = m_UniformBuffers[i]->DescriptorInfo();

			DescriptorWriter(*m_GlobalSetLayouts[0], *m_GlobalPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(m_GlobalDescriptorSets[i]);
		}
		std::vector<VkDescriptorSetLayout> setLayouts;
		for (int i = 0; i < m_GlobalSetLayouts.size(); i++)
			setLayouts.push_back(m_GlobalSetLayouts[i]->GetDescriptorSetLayout());

	}

	void VulkanContext::BeginFrame()
	{
		VkCommandBuffer commandBuffer = GetRenderer().BeginFrame();
		GetRenderer().BeginSwapChainRenderPass(commandBuffer);
	}

	void VulkanContext::EndFrame()
	{
		GetRenderer().EndSwapChainRenderPass(GetRenderer().GetCurrentCommandBuffer());
		GetRenderer().EndFrame();
	}
}