#include "Mfepch.h"
#include "VulkanMaterial.h"

#include "MyFirstEngine/Application.h"
#include "VulkanContext.h"
#include "VulkanShader.h"

namespace MyFirstEngine
{
	MyFirstEngine::VulkanMaterial::VulkanMaterial(const std::shared_ptr<Shader> shader)
		: m_Device(static_cast<MyFirstEngine::VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext())->GetDevice())
	{
		this->m_Shader = shader;
	}

	MyFirstEngine::VulkanMaterial::~VulkanMaterial()
	{
		vkDestroyPipelineLayout(m_Device.GetLogicalDevice(), m_PipelineLayout, nullptr);
	}

	void MyFirstEngine::VulkanMaterial::Bind() const
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());
		VkCommandBuffer commandBuffer = graphicsContext->GetRenderer().GetCurrentCommandBuffer();

		m_Pipeline->BindCommandBuffer(commandBuffer);

		VkDescriptorSet globalSet = graphicsContext->GetGlobalSet(graphicsContext->GetRenderer().GetFrameIndex());
		vkCmdBindDescriptorSets(
			commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout,
			0, 1, &globalSet, 0, nullptr
		);

		if (m_TextureSet != VK_NULL_HANDLE)
		{
			vkCmdBindDescriptorSets(
				commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout,
				1, 1, &m_TextureSet, 0, nullptr
			);
		}

		if (GetPushConstantsSize() == 0)
			return;


		std::vector<uint8_t> pushConstantData;
		for (auto& [key, value] : m_PushConstants) 
		{ 
			pushConstantData.insert(pushConstantData.end(), value.data.begin(), value.data.end());
		}

		vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, GetPushConstantsSize(), pushConstantData.data());
	}


	void MyFirstEngine::VulkanMaterial::Unbind() const
	{

	}

	uint32_t VulkanMaterial::GetPushConstantsSize() const
	{
		uint32_t size = 0;
		for (auto& [key, value] : m_PushConstants)
		{
			size += static_cast<uint32_t>(value.data.size());
		}

		return size;
	}

	void VulkanMaterial::CreatePipeline(VertexArray vertexArray)
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());
		VkRenderPass renderPass = graphicsContext->GetRenderer().GetSwapChainRenderPass();
		
		CreateTexturesSet();

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		for (int i = 0; i < graphicsContext->GetGlobalSetLayout().size(); i++)
		{
			descriptorSetLayouts.push_back(graphicsContext->GetGlobalSetLayout()[i]->GetDescriptorSetLayout());
		}

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		uint32_t pushConstantsSize = GetPushConstantsSize();
		if (pushConstantsSize == 0)
		{
			pipelineLayoutInfo.pushConstantRangeCount = 0;
			pipelineLayoutInfo.pPushConstantRanges = nullptr;
		}	
		else
		{
			VkPushConstantRange pushConstantRange = {};
			pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			pushConstantRange.offset = 0;
			pushConstantRange.size = pushConstantsSize;

			pipelineLayoutInfo.pushConstantRangeCount = 1;
			pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		}

		MFE_CORE_ASSERT(vkCreatePipelineLayout(m_Device.GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) == VK_SUCCESS, "Failed to create pipeline layout");

		PipelineConfig pipelineConfig{};
		Pipeline::DefaultConfigInfo(pipelineConfig);
		pipelineConfig.bindingDescriptions = static_cast<VulkanVertexArray*>(&vertexArray)->GetBindingDescriptions();
		pipelineConfig.attributeDescriptions = static_cast<VulkanVertexArray*>(&vertexArray)->GetAttributeDescriptions();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		pipelineConfig.multisampling.rasterizationSamples = m_Device.GetMaxUsableSampleCount();
		m_Pipeline = std::make_unique<Pipeline>(m_Device, static_cast<VulkanShader*>(&(*m_Shader))->GetShaderModule(0), static_cast<VulkanShader*>(&(*m_Shader))->GetShaderModule(1), pipelineConfig);
	}

	void VulkanMaterial::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		MFE_CORE_ASSERT(vkCreateShaderModule(m_Device.GetLogicalDevice(), &createInfo, nullptr, shaderModule) == VK_SUCCESS, "Failed to create shader module!");
	}

	std::unique_ptr<Material> VulkanMaterial::CreateMatFromShader(const std::shared_ptr<Shader> shader)
	{
		return std::make_unique<VulkanMaterial>(shader);
	}

	void VulkanMaterial::CreateTexturesSet()
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());

		DescriptorWriter descriptorWritter = DescriptorWriter(*graphicsContext->GetGlobalSetLayout()[1], graphicsContext->GetGlobalPool());
		for (auto texture : m_Textures)
		{
			VkDescriptorImageInfo imageInfo = static_cast<VulkanTexture*>(texture.second.get())->GetImageInfo();
			descriptorWritter.WriteImage(0, &imageInfo);
		}
		descriptorWritter.Build(m_TextureSet);
	}
}