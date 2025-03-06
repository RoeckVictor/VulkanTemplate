#include "Mfepch.h"
#include "MyFirstEngine/Application.h"
#include "VulkanContext.h"

#include "VulkanShader.h"

#include <shaderc/shaderc.h>

namespace MyFirstEngine
{
	VulkanShader::VulkanShader(std::vector<std::vector<char>> shaderCodes)
		: m_Device(static_cast<MyFirstEngine::VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext())->GetDevice())
	{
		m_ShaderCodes = shaderCodes;
		for(const std::vector<char>& code : m_ShaderCodes)
		{
			SpvReflectShaderModule reflectModule;
			MFE_ASSERT(spvReflectCreateShaderModule(code.size(), code.data(), &reflectModule) == SPV_REFLECT_RESULT_SUCCESS,
					   "Failed to create shader reflect module");
			m_ReflectModules.push_back(reflectModule);

			// Initialize SPIRV-Cross compiler
			std::vector<uint32_t> spirv(reinterpret_cast<const uint32_t*>(code.data()),
									  reinterpret_cast<const uint32_t*>(code.data() + code.size()));
			m_Compilers.push_back(std::make_unique<spirv_cross::Compiler>(spirv));
			m_Resources.push_back(std::nullopt);

			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

			VkShaderModule shaderModule;
			MFE_CORE_ASSERT(vkCreateShaderModule(m_Device.GetLogicalDevice(), &createInfo, nullptr, &shaderModule) == VK_SUCCESS, 
						   "Failed to create shader module!");
			m_ShaderModules.push_back(shaderModule);
		}
		ReflectShaderProperties();
	}

	VulkanShader::~VulkanShader()
	{
		for (VkShaderModule shaderModule : m_ShaderModules)
		{
			vkDestroyShaderModule(m_Device.GetLogicalDevice(), shaderModule, nullptr);
		}

		for (SpvReflectShaderModule reflectModule : m_ReflectModules)
		{
			spvReflectDestroyShaderModule(&reflectModule);
		}
	}

	std::shared_ptr<Shader> VulkanShader::CreateShaderFromFiles(const std::vector<std::string> shaderFiles)
	{
		// shaderc::Compiler compiler;
		return std::make_shared<VulkanShader>(std::vector<std::vector<char>>());
	}

	std::shared_ptr<Shader> VulkanShader::CreateShaderFromCompiledFiles(const std::vector<std::string> compiledShaderFiles)
	{
		std::vector<std::vector<char>> filesData;
		for (std::string filePath : compiledShaderFiles)
		{
			filesData.push_back(Shader::GetFileData(filePath));
		}
		
		return std::make_shared<VulkanShader>(filesData);
	}

	void VulkanShader::ReflectShaderProperties()
	{
		for (uint32_t stageIndex = 0; stageIndex < GetShaderStageCount(); stageIndex++)
		{
			// Determine shader stage
			uint32_t currentStage = (stageIndex == 0) ? 
				VK_SHADER_STAGE_VERTEX_BIT : 
				VK_SHADER_STAGE_FRAGMENT_BIT;

			const spirv_cross::ShaderResources& resources = GetShaderResources(stageIndex);

			// Reflect Uniform Buffers
			for (const auto& buffer : resources.uniform_buffers) {
				ShaderProperty prop;
				prop.name = buffer.name;
				prop.binding = GetResourceBinding(buffer, stageIndex);
				prop.set = GetResourceSet(buffer, stageIndex);
				prop.isPushConstant = false;
				prop.usage = ShaderProperty::Usage::Uniform;

				const spirv_cross::SPIRType& type = GetResourceType(buffer, stageIndex);
				for (uint32_t i = 0; i < type.member_types.size(); i++) {
					const std::string& memberName = GetMemberName(buffer, i, stageIndex);
					const spirv_cross::SPIRType& memberType = GetType(type.member_types[i], stageIndex);

					ShaderProperty memberProp;
					memberProp.name = memberName;
					memberProp.binding = prop.binding;
					memberProp.set = prop.set;
					memberProp.isPushConstant = false;
					memberProp.stages = currentStage;
					memberProp.usage = ShaderProperty::Usage::Uniform;

					auto reflectedType = ReflectType(memberType, *m_Compilers[stageIndex], i);
					memberProp.type = reflectedType.type;
					memberProp.arraySizes = reflectedType.arraySizes;
					memberProp.members = reflectedType.members;

					if (m_Properties.find(memberProp.name) != m_Properties.end()) {
						m_Properties[memberProp.name].stages |= currentStage;
					} else {
						m_Properties[memberProp.name] = memberProp;
					}
				}
			}

			// Reflect Textures
			for (const auto& texture : resources.sampled_images) {
				ShaderProperty prop;
				prop.name = texture.name;
				prop.binding = GetResourceBinding(texture, stageIndex);
				prop.set = GetResourceSet(texture, stageIndex);
				prop.type = ShaderProperty::Type::Texture2D;
				prop.isPushConstant = false;
				prop.stages = currentStage;
				prop.usage = ShaderProperty::Usage::Sampler;

				if (m_Properties.find(prop.name) != m_Properties.end()) {
					m_Properties[prop.name].stages |= currentStage;
				} else {
					m_Properties[prop.name] = prop;
				}
			}

			// Reflect Push Constants
			for (const auto& pushConstant : resources.push_constant_buffers) {
				const spirv_cross::SPIRType& type = GetResourceType(pushConstant, stageIndex);
				for (uint32_t i = 0; i < type.member_types.size(); i++) {
					const std::string& memberName = GetMemberName(pushConstant, i, stageIndex);
					const spirv_cross::SPIRType& memberType = GetType(type.member_types[i], stageIndex);

					ShaderProperty prop;
					prop.name = memberName;
					prop.isPushConstant = true;
					prop.binding = 0;  // Push constants don't use bindings
					prop.set = 0;      // Push constants don't use descriptor sets
					prop.stages = currentStage;
					prop.usage = ShaderProperty::Usage::PushConstant;

					auto reflectedType = ReflectType(memberType, *m_Compilers[stageIndex], i);
					prop.type = reflectedType.type;
					prop.arraySizes = reflectedType.arraySizes;
					prop.members = reflectedType.members;

					if (m_Properties.find(prop.name) != m_Properties.end()) {
						m_Properties[prop.name].stages |= currentStage;
					} else {
						m_Properties[prop.name] = prop;
					}
				}
			}

			// Reflect Input Variables
			for (const auto& input : resources.stage_inputs) {
				ShaderProperty prop;
				prop.name = input.name;
				prop.binding = 0;
				prop.set = 0;
				prop.isPushConstant = false;
				prop.stages = currentStage;
				prop.usage = ShaderProperty::Usage::Input;

				const spirv_cross::SPIRType& type = GetResourceType(input, stageIndex);
				auto reflectedType = ReflectType(type, *m_Compilers[stageIndex], 0);
				prop.type = reflectedType.type;
				prop.arraySizes = reflectedType.arraySizes;
				prop.members = reflectedType.members;

				if (m_Properties.find(prop.name) != m_Properties.end()) {
					m_Properties[prop.name].stages |= currentStage;
				} else {
					m_Properties[prop.name] = prop;
				}
			}

			// Reflect Output Variables
			for (const auto& output : resources.stage_outputs) {
				ShaderProperty prop;
				prop.name = output.name;
				prop.binding = 0;
				prop.set = 0;
				prop.isPushConstant = false;
				prop.stages = currentStage;
				prop.usage = ShaderProperty::Usage::Output;

				const spirv_cross::SPIRType& type = GetResourceType(output, stageIndex);
				auto reflectedType = ReflectType(type, *m_Compilers[stageIndex]);
				prop.type = reflectedType.type;
				prop.arraySizes = reflectedType.arraySizes;
				prop.members = reflectedType.members;

				if (m_Properties.find(prop.name) != m_Properties.end()) {
					m_Properties[prop.name].stages |= currentStage;
				} else {
					m_Properties[prop.name] = prop;
				}
			}
		}
	}

	ShaderProperty::StructMember VulkanShader::ReflectType(const spirv_cross::SPIRType& type, const spirv_cross::Compiler& compiler, uint32_t memberIndex) const {
		ShaderProperty::StructMember member;
		
		// Handle arrays (now supporting multiple dimensions)
		if (type.array.size() > 0) {
			member.type = ShaderProperty::Type::Array;
			member.arraySizes.assign(type.array.begin(), type.array.end());
			
			auto baseType = type;
			baseType.array.clear();
			member.members.push_back(ReflectType(baseType, compiler, 0));
			return member;
		}

		// Handle structs
		if (type.basetype == spirv_cross::SPIRType::Struct) {
			member.type = ShaderProperty::Type::Struct;
			for (uint32_t i = 0; i < type.member_types.size(); i++) {
				const auto& memberType = compiler.get_type(type.member_types[i]);
				const auto& memberName = compiler.get_member_name(type.self, i);
				auto submember = ReflectType(memberType, compiler, i);
				submember.name = memberName;
				member.members.push_back(submember);
			}
			return member;
		}

		// Handle basic types
		if (type.vecsize == 1 && type.columns == 1) {
			member.type = ShaderProperty::Type::Float;
		}
		else if (type.vecsize > 1 && type.columns == 1) {
			switch (type.vecsize) {
				case 2: member.type = ShaderProperty::Type::Float2; break;
				case 3: member.type = ShaderProperty::Type::Float3; break;
				case 4: member.type = ShaderProperty::Type::Float4; break;
			}
		}
		else if (type.columns > 1) {
			switch (type.columns) {
				case 3: member.type = ShaderProperty::Type::Mat3; break;
				case 4: member.type = ShaderProperty::Type::Mat4; break;
			}
		}
		
		return member;
	}
}