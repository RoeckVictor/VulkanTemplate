#include "Mfepch.h"
#include "Material.h"

#include "VulkanRenderer\VulkanMaterial.h"

#include "Renderer.h"


namespace MyFirstEngine
{
	std::unique_ptr<Material> Material::CreateMatFromFile(const std::string& vertPath, const std::string& fragPath)
	{
		switch (Renderer::GetSelectedAPI())
		{
		case RendererAPI::SelectedAPI::None: return nullptr;
		case RendererAPI::SelectedAPI::Vulkan: 
			VulkanContext* graphicsContext = static_cast<MyFirstEngine::VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());
			return VulkanMaterial::CreateMatFromFile(graphicsContext->GetDevice(), vertPath, fragPath);
		}

		MFE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	void Material::AddUniform(const uint32_t id, const std::string& name, const std::vector<uint8_t>& data, const bool isPushConstant)
	{
		switch (Renderer::GetSelectedAPI())
		{
		case RendererAPI::SelectedAPI::None: break;
		case RendererAPI::SelectedAPI::Vulkan: (isPushConstant) ? pushConstants[id] = { name, data } : uniforms[id] = { name, data }; break;
		}
	}

	void Material::UpdateUniform(const uint32_t id, const std::vector<uint8_t>& data, const bool isPushConstant)
	{
		switch (Renderer::GetSelectedAPI())
		{
		case RendererAPI::SelectedAPI::None: break;
		case RendererAPI::SelectedAPI::Vulkan: (isPushConstant) ? pushConstants[id].data = data : uniforms[id].data = data; break;
		}
	}

	void Material::RemoveUniform(const uint32_t id, const bool isPushConstant)
	{
		switch (Renderer::GetSelectedAPI())
		{
		case RendererAPI::SelectedAPI::None: break;
		case RendererAPI::SelectedAPI::Vulkan: (isPushConstant) ? pushConstants.erase(id) : uniforms.erase(id); break;
		}
	}
}