#pragma once

#include <vulkan/vulkan.h>
#include <spirv_reflect.h>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <optional>

#include "Shader.h"

namespace MyFirstEngine 
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(std::vector<std::vector<char>> shaderCodes);
		~VulkanShader();

		static std::shared_ptr<Shader> CreateShaderFromFiles(const std::vector<std::string> shaderFiles);
		static std::shared_ptr<Shader> CreateShaderFromCompiledFiles(const std::vector<std::string> compiledShaderFiles);

		SpvReflectShaderModule GetReflectModule(uint32_t index) { return m_ReflectModules[index]; };
		VkShaderModule GetShaderModule(uint32_t index) { return m_ShaderModules[index]; };

		uint32_t GetShaderStageCount() const { return static_cast<uint32_t>(m_ShaderModules.size()); }

		const spirv_cross::ShaderResources& GetShaderResources(uint32_t index) {
			if (!m_Resources[index].has_value()) {
				m_Resources[index] = m_Compilers[index]->get_shader_resources();
			}
			return m_Resources[index].value();
		}

		uint32_t GetResourceBinding(const spirv_cross::Resource& resource, uint32_t index) const { 
			return m_Compilers[index]->get_decoration(resource.id, spv::DecorationBinding); 
		}

		uint32_t GetResourceSet(const spirv_cross::Resource& resource, uint32_t index) const { 
			return m_Compilers[index]->get_decoration(resource.id, spv::DecorationDescriptorSet); 
		}

		const spirv_cross::SPIRType& GetResourceType(const spirv_cross::Resource& resource, uint32_t index) const { 
			return m_Compilers[index]->get_type(resource.type_id); 
		}

		std::string GetMemberName(const spirv_cross::Resource& resource, uint32_t memberIndex, uint32_t shaderIndex) const { 
			return m_Compilers[shaderIndex]->get_member_name(resource.base_type_id, memberIndex); 
		}

		const spirv_cross::SPIRType& GetType(uint32_t id, uint32_t index) const { 
			return m_Compilers[index]->get_type(id); 
		}

	private:
		void ReflectShaderProperties();
		ShaderProperty::StructMember ReflectType(const spirv_cross::SPIRType& type, const spirv_cross::Compiler& compiler, uint32_t memberIndex = 0) const;
		Device& m_Device;
		std::vector<SpvReflectShaderModule> m_ReflectModules;
		std::vector<VkShaderModule> m_ShaderModules;
		std::vector<std::unique_ptr<spirv_cross::Compiler>> m_Compilers;
		mutable std::vector<std::optional<spirv_cross::ShaderResources>> m_Resources;
	};
}