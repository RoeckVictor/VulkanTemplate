#include "Mfepch.h"
#include "Model.h"
#include "VulkanRenderer\VulkanModel.h"

#include "Renderer.h"

namespace MyFirstEngine
{
	std::unique_ptr<Model> MyFirstEngine::Model::CreateModelFromFile(const std::string& filepath)
	{
		switch (Renderer::GetSelectedAPI())
		{
			case SelectedAPI::None: return nullptr;
			case SelectedAPI::Vulkan: return VulkanModel::CreateModelFromFile(filepath);
		}

		MFE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	std::unique_ptr<Model> MyFirstEngine::Model::CreateModelFromData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		switch (Renderer::GetSelectedAPI())
		{
			case SelectedAPI::None: return nullptr;
			case SelectedAPI::Vulkan: return VulkanModel::CreateModelFromData(vertices, indices);
		}

		MFE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}
