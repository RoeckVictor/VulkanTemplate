#include "Mfepch.h"
#include "Model.h"
#include "VulkanRenderer\VulkanModel.h"

#include "Renderer.h"

namespace MyFirstEngine
{
	std::unique_ptr<Model> MyFirstEngine::Model::CreateModelFromFile(const std::string& filepath, const VertexLayout layout)
	{
		switch (Renderer::GetSelectedAPI())
		{
			case RendererAPI::SelectedAPI::None: return nullptr;
			case RendererAPI::SelectedAPI::Vulkan: return VulkanModel::CreateModelFromFile(filepath, layout);
		}

		MFE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	std::unique_ptr<Model> MyFirstEngine::Model::CreateModelFromData(const VertexArray vertices, const std::vector<uint32_t>& indices)
	{
		switch (Renderer::GetSelectedAPI())
		{
			case RendererAPI::SelectedAPI::None: return nullptr;
			case RendererAPI::SelectedAPI::Vulkan: return nullptr; // VulkanModel::CreateModelFromData(vertices, indices);
		}

		MFE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}
