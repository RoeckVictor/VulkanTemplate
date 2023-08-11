#include "Mfepch.h"
#include "VulkanMaterial.h"

#include "MyFirstEngine/Application.h"
#include "VulkanContext.h"

namespace MyFirstEngine
{
	MyFirstEngine::VulkanMaterial::VulkanMaterial(Device& device, const std::string& vertPath, const std::string& fragPath)
		: device(device)
	{
		auto vertCode = ReadShaderFile(vertPath);
		auto fragCode = ReadShaderFile(fragPath);

		CreateShaderModule(vertCode, &vertShaderModule);
		CreateShaderModule(fragCode, &fragShaderModule);
	}

	MyFirstEngine::VulkanMaterial::~VulkanMaterial()
	{
		vkDestroyShaderModule(device.device(), fragShaderModule, nullptr);
		vkDestroyShaderModule(device.device(), vertShaderModule, nullptr);
		vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
	}

	void MyFirstEngine::VulkanMaterial::Bind() const
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());
		VkCommandBuffer commandBuffer = graphicsContext->GetRenderer().GetCurrentCommandBuffer();

		pipeline->BindCommandBuffer(commandBuffer);

		VkDescriptorSet globalSet = graphicsContext->GetGlobalSet(graphicsContext->GetRenderer().GetFrameIndex());
		vkCmdBindDescriptorSets(
			commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
			0, 1, &globalSet, 0, nullptr
		);

		if (textureSet != VK_NULL_HANDLE)
		{
			vkCmdBindDescriptorSets(
				commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
				1, 1, &textureSet, 0, nullptr
			);
		}

		if (GetPushConstantsSize() == 0)
			return;


		std::vector<uint8_t> pushConstantData;
		for (auto& [key, value] : pushConstants) 
		{ 
			pushConstantData.insert(pushConstantData.end(), value.data.begin(), value.data.end());
		}

		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, GetPushConstantsSize(), pushConstantData.data());
	}


	void MyFirstEngine::VulkanMaterial::Unbind() const
	{

	}

	uint32_t VulkanMaterial::GetPushConstantsSize() const
	{
		uint32_t size = 0;
		for (auto& [key, value] : pushConstants)
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
			descriptorSetLayouts.push_back(graphicsContext->GetGlobalSetLayout()[i]->GetDescriptorSetLayout());

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

		if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout");

		PipelineConfig pipelineConfig{};
		Pipeline::DefaultConfigInfo(pipelineConfig);
		pipelineConfig.bindingDescriptions = static_cast<VulkanVertexArray*>(&vertexArray)->GetBindingDescriptions();
		pipelineConfig.attributeDescriptions = static_cast<VulkanVertexArray*>(&vertexArray)->GetAttributeDescriptions();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipelineConfig.multisampling.rasterizationSamples = device.GetMaxUsableSampleCount();
		pipeline = std::make_unique<Pipeline>(device, vertShaderModule, fragShaderModule, pipelineConfig);
	}

	void VulkanMaterial::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module!");
	}

	std::unique_ptr<Material> VulkanMaterial::CreateMatFromFile(Device& device, const std::string& vertPath, const std::string& fragPath)
	{
		return std::make_unique<VulkanMaterial>(device, vertPath, fragPath);
	}

	std::vector<char> VulkanMaterial::ReadShaderFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			throw std::runtime_error("Failed to open file: " + path);

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	void VulkanMaterial::CreateTexturesSet()
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());

		DescriptorWriter descriptorWritter = DescriptorWriter(*graphicsContext->GetGlobalSetLayout()[1], graphicsContext->GetGlobalPool());
		for (auto texture : textures)
		{
			VkDescriptorImageInfo imageInfo = texture.second->GetImageInfo();
			descriptorWritter.WriteImage(0, &imageInfo);
		}
		descriptorWritter.Build(textureSet);
	}
}