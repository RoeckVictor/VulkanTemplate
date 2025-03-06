#include "Mfepch.h"
#include "Material.h"

#include "VulkanRenderer\VulkanMaterial.h"

#include "Renderer.h"

namespace MyFirstEngine
{
	std::unique_ptr<Material> Material::CreateMatFromShader(const std::shared_ptr<Shader> shader)
	{
		switch (Renderer::GetSelectedAPI())
		{
			case RendererAPI::SelectedAPI::None: return nullptr;
			case RendererAPI::SelectedAPI::Vulkan: 
				return VulkanMaterial::CreateMatFromShader(shader);
		}

		MFE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	void Material::AddUniform(const uint32_t id, const std::string& name, const std::vector<uint8_t>& data, const bool isPushConstant)
	{
		switch (Renderer::GetSelectedAPI())
		{
			case RendererAPI::SelectedAPI::None: break;
			case RendererAPI::SelectedAPI::Vulkan: (isPushConstant) ? m_PushConstants[id] = { name, data } : m_Uniforms[id] = { name, data }; break;
		}
	}

	void Material::UpdateUniform(const uint32_t id, const std::vector<uint8_t>& data, const bool isPushConstant)
	{
		switch (Renderer::GetSelectedAPI())
		{
			case RendererAPI::SelectedAPI::None: break;
			case RendererAPI::SelectedAPI::Vulkan: (isPushConstant) ? m_PushConstants[id].data = data : m_Uniforms[id].data = data; break;
		}
	}

	void Material::RemoveUniform(const uint32_t id, const bool isPushConstant)
	{
		switch (Renderer::GetSelectedAPI())
		{
			case RendererAPI::SelectedAPI::None: break;
			case RendererAPI::SelectedAPI::Vulkan: (isPushConstant) ? m_PushConstants.erase(id) : m_Uniforms.erase(id); break;
		}
	}
}