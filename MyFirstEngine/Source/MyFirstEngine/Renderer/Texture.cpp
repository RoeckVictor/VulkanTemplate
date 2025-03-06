#include "Mfepch.h"

#include "Renderer.h"

#include "Texture.h"
#include "VulkanRenderer\VulkanTexture.h"

namespace MyFirstEngine 
{
	std::shared_ptr<Texture> Texture::CreateFromFile(const std::string& filename)
	{
		switch (Renderer::GetSelectedAPI())
		{
			case RendererAPI::SelectedAPI::None: return nullptr;
			case RendererAPI::SelectedAPI::Vulkan: return VulkanTexture::CreateTextureFromFile(filename);
		}

		MFE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}