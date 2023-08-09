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

	void Material::AddUniform(const std::string& name, size_t size, void* data, const bool isPushConstant)
	{
		switch (Renderer::GetSelectedAPI())
		{
		case RendererAPI::SelectedAPI::None: break;
		case RendererAPI::SelectedAPI::Vulkan: (isPushConstant) ? pushConstants[name] = { name, size, data } : uniforms[name] = { name, size, data }; break;
		}
	}

	void Material::RemoveUniform(const std::string& name, const bool isPushConstant)
	{
		switch (Renderer::GetSelectedAPI())
		{
		case RendererAPI::SelectedAPI::None: break;
		case RendererAPI::SelectedAPI::Vulkan: (isPushConstant) ? pushConstants.erase("name") : uniforms.erase("name"); break;
		}
	}


}