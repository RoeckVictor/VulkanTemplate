#include "Mfepch.h"
#include "VulkanMaterial.h"
#include <spirv_cross/spirv_cross.hpp>

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
		auto material = std::make_unique<VulkanMaterial>(shader);
		material->PrintShaderProperties();
		return material;
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

	void PrintStructMember(const ShaderProperty::StructMember& member, int indent = 0) {
		std::string indentation(indent * 2, ' ');
		
		if (member.type == ShaderProperty::Type::Array) {
			std::string arrayDims;
			for (uint32_t dim : member.arraySizes) {
				arrayDims += "[" + std::to_string(dim) + "]";
			}
			MFE_CORE_INFO("{}Array{} of:", indentation, arrayDims);
			PrintStructMember(member.members[0], indent + 1);
		}
		else if (member.type == ShaderProperty::Type::Struct) {
			MFE_CORE_INFO("{}Struct containing:", indentation);
			for (const auto& submember : member.members) {
				MFE_CORE_INFO("{}  {}: ", indentation, submember.name);
				PrintStructMember(submember, indent + 2);
			}
		}
		else {
			MFE_CORE_INFO("{}{}", indentation, static_cast<int>(member.type));
		}
	}

	void VulkanMaterial::PrintShaderProperties() const
	{
		auto* shader = static_cast<VulkanShader*>(m_Shader.get());
		if (!shader) {
			MFE_CORE_ERROR("No shader attached to material");
			return;
		}

		const auto& properties = shader->GetProperties();
		MFE_CORE_INFO("Shader Properties:");
		for (const auto& [name, prop] : properties) {
			MFE_CORE_INFO("  Property: {0}", name);
			MFE_CORE_INFO("    Type: {0}", static_cast<int>(prop.type));
			MFE_CORE_INFO("    Stage: 0x{0:X} ({1})", prop.stages, 
				prop.stages == VK_SHADER_STAGE_VERTEX_BIT ? "Vertex" : 
				prop.stages == VK_SHADER_STAGE_FRAGMENT_BIT ? "Fragment" : 
				prop.stages == (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT) ? "Vertex|Fragment" : 
				"Unknown");
			MFE_CORE_INFO("    Set: {0}, Binding: {1}", prop.set, prop.binding);
			MFE_CORE_INFO("    Is Push Constant: {0}", prop.isPushConstant);
			MFE_CORE_INFO("    Usage: {0}", 
				prop.usage == ShaderProperty::Usage::Input ? "Input" :
				prop.usage == ShaderProperty::Usage::Output ? "Output" :
				prop.usage == ShaderProperty::Usage::Uniform ? "Uniform" :
				prop.usage == ShaderProperty::Usage::PushConstant ? "Push Constant" :
				prop.usage == ShaderProperty::Usage::Sampler ? "Sampler" : "Unknown");
			if (prop.type == ShaderProperty::Type::Struct || prop.type == ShaderProperty::Type::Array) {
				MFE_CORE_INFO("    Members:");
				PrintStructMember(ShaderProperty::StructMember{prop.name, prop.type, prop.arraySizes, prop.members}, 3);
			}
		}
	}
}