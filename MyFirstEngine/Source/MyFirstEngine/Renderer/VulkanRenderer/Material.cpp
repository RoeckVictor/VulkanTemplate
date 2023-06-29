#include "Mfepch.h"
#include "Material.h"

#include "MyFirstEngine/Application.h"
#include "VulkanContext.h"

namespace MyFirstEngine
{
	MyFirstEngine::Material::Material(Device& device, const std::string& vertPath, const std::string& fragPath)
		: device(device)
	{
		auto vertCode = ReadShaderFile(vertPath);
		auto fragCode = ReadShaderFile(fragPath);

		CreateShaderModule(vertCode, &vertShaderModule);
		CreateShaderModule(fragCode, &fragShaderModule);
	}

	MyFirstEngine::Material::~Material()
	{
		vkDestroyShaderModule(device.device(), fragShaderModule, nullptr);
		vkDestroyShaderModule(device.device(), vertShaderModule, nullptr);
		vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
	}

	void MyFirstEngine::Material::Bind() const
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

		std::vector<void*> push = {};
		for (auto& pushConstant : pushConstants)
		{
			push.push_back(pushConstant.data);
		}

		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, GetPushConstantsSize(), push.data());
	}

	void MyFirstEngine::Material::Unbind() const
	{

	}

	void Material::AddPushConstant(size_t size, void* data)
	{
		PushConstant pushConstant;
		pushConstant.size = size;
		pushConstant.data = data;
		pushConstants.push_back(pushConstant);
	}

	void Material::RemovePushConstant(size_t index)
	{
		if (index < pushConstants.size()) 
		{
			pushConstants.erase(pushConstants.begin() + index);
		}
	}

	uint32_t Material::GetPushConstantsSize() const
	{
		uint32_t size = 0;
		for (auto& pushConstant : pushConstants)
		{
			size += static_cast<uint32_t>(pushConstant.size);
		}
		return size;
	}

	void Material::CreatePipeline(std::vector<VkVertexInputBindingDescription> bindingDescriptions, 
								  std::vector<VkVertexInputAttributeDescription> attributeDescriptions)
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());
		VkRenderPass renderPass = graphicsContext->GetRenderer().GetSwapChainRenderPass();
		
		CreateTexturesSet();

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		for (int i = 0; i < graphicsContext->GetGlobalSetLayout().size(); i++)
			descriptorSetLayouts.push_back(graphicsContext->GetGlobalSetLayout()[i]->GetDescriptorSetLayout());

		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = GetPushConstantsSize();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout");

		PipelineConfig pipelineConfig{};
		Pipeline::DefaultConfigInfo(pipelineConfig);
		pipelineConfig.bindingDescriptions = bindingDescriptions;
		pipelineConfig.attributeDescriptions = attributeDescriptions;
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipelineConfig.multisampling.rasterizationSamples = device.GetMaxUsableSampleCount();
		pipeline = std::make_unique<Pipeline>(device, vertShaderModule, fragShaderModule, pipelineConfig);
	}

	void Material::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module!");
	}

	std::unique_ptr<Material> Material::CreateMatFromFile(Device& device, const std::string& vertPath, const std::string& fragPath)
	{
		return std::make_unique<Material>(device, vertPath, fragPath);
	}

	std::vector<char> Material::ReadShaderFile(const std::string& path)
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

	void Material::CreateTexturesSet()
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